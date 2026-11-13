"""Device profiles (runtime/cfg.c, "device profiles"): which machine each start recognises and the defaults it gets,
without game files or a GPU.

The real cfg.c is compiled with stand-ins for the host, and each case runs it with DKB_DEVICE_SYSROOT pointing at a
folder that holds the /sys and /proc files of the machine it pretends to be.

Usage: python tests/device_profile_test.py [--cc /path/to/clang]
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

src = Path(__file__).resolve().parents[1]
ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument('--cc', default=os.environ.get('CC') or shutil.which('clang'))
args = ap.parse_args()
if not args.cc:
    ap.error('pass --cc with the project Clang executable')
if os.name == 'nt':
    print('SKIP: the device profiles read Linux /sys and /proc files')
    sys.exit(0)
out = src / 'build' / 'tests' / 'device-profile'
out.mkdir(parents=True, exist_ok=True)

HARNESS = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg.h"
#include "platform/platform.h"
const int rt_game_version = 0;   // Bananza
void plat_mutex_lock(PlatMutex *m) { (void)m; }
void plat_mutex_unlock(PlatMutex *m) { (void)m; }
void rt_fatal(const char *f, ...) { (void)f; abort(); }
void rt_log(const char *f, ...) { (void)f; }
uint8_t *plat_file_read(const char *p, uint64_t *s) { (void)p; (void)s; return NULL; }
int plat_file_exists(const char *p) { (void)p; return 0; }
int64_t plat_file_size(const char *p) { (void)p; return -1; }
int plat_file_write_atomic(const char *p, const void *d, size_t n) { (void)p; (void)d; (void)n; return 0; }
int main(int argc, char **argv) {
    printf("profile=%s\n", cfg_device_profile());
    for (int i = 1; i < argc; i++) {
        const char *v = cfg_get(argv[i]);
        printf("%s=%s\n", argv[i], v ? v : "(unset)");
    }
    return 0;
}
'''
(out / 'harness.c').write_text(HARNESS)
exe = out / 'harness'
r = subprocess.run([args.cc, '-O1', '-fno-strict-aliasing', '-ffunction-sections', '-fdata-sections', '-fuse-ld=lld',
                    '-Wl,--gc-sections', '-pthread', '-I', str(src / 'runtime'), str(out / 'harness.c'),
                    str(src / 'runtime' / 'cfg.c'), '-o', str(exe)], capture_output=True, text=True)
if r.returncode:
    sys.exit(f'compile failed:\n{r.stderr[-4000:]}')

GIB = 1 << 30
KEYS = ['TOTK_OUTPUT_RES', 'TOTK_WINDOW_SIZE', 'TOTK_OPERATION_MODE', 'TOTK_FPS_LIMIT', 'TOTK_FULLSCREEN',
        'TOTK_FRAME_GEN', 'TOTK_VK_PRESENT']


def machine(root, dmi=None, cpu=None, ram_gib=None, cards=()):
    """root as a machine: firmware fields, CPU name, RAM and graphics cards as (vendor, video memory bytes or None)."""
    for name, value in (dmi or {}).items():
        p = root / 'sys/class/dmi/id' / name
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(value + '\n')
    if cpu:
        (root / 'proc').mkdir(parents=True, exist_ok=True)
        (root / 'proc/cpuinfo').write_text(''.join(f'processor\t: {i}\nvendor_id\t: AuthenticAMD\nmodel name\t: {cpu}\n\n'
                                                   for i in range(4)))
    if ram_gib:
        (root / 'proc').mkdir(parents=True, exist_ok=True)
        (root / 'proc/meminfo').write_text(f'MemTotal:       {int(ram_gib * 1048576)} kB\nMemFree:        1000 kB\n')
    for i, (vendor, vram) in enumerate(cards):
        d = root / f'sys/class/drm/card{i}/device'
        d.mkdir(parents=True, exist_ok=True)
        (d / 'vendor').write_text(vendor + '\n')
        if vram is not None:
            (d / 'mem_info_vram_total').write_text(f'{vram}\n')


def run(root, env_extra=None):
    env = {k: v for k, v in os.environ.items() if k not in ('SteamDeck',) and not k.startswith(('TOTK_', 'DKB_'))}
    env['DKB_DEVICE_SYSROOT'] = str(root)
    env.update(env_extra or {})
    r = subprocess.run([str(exe), *KEYS], env=env, capture_output=True, text=True, timeout=20, cwd=root)
    assert r.returncode == 0, r.stderr
    return dict(line.split('=', 1) for line in r.stdout.splitlines())


CASES = [
    # name, machine, environment, expected profile, expected values
    ('steam deck lcd', dict(dmi={'board_vendor': 'Valve', 'product_name': 'Jupiter'}, cpu='AMD Custom APU 0405',
                            ram_gib=14.5, cards=[('0x1002', GIB)]), {},
     'deck', {'TOTK_OUTPUT_RES': '1280x800', 'TOTK_OPERATION_MODE': 'handheld', 'TOTK_FPS_LIMIT': '40',
              'TOTK_FRAME_GEN': '(unset)', 'TOTK_VK_PRESENT': 'fifo'}),
    ('steam deck oled by cpu only', dict(cpu='AMD Custom APU 0932', ram_gib=14.5), {},
     'deck', {'TOTK_WINDOW_SIZE': '1280x800'}),
    ('steam deck by steam', dict(ram_gib=64), {'SteamDeck': '1'}, 'deck', {'TOTK_FULLSCREEN': '1'}),
    ('legion go s by firmware', dict(dmi={'sys_vendor': 'LENOVO', 'product_name': '83L3',
                                          'product_family': 'Legion Go S 8APU1', 'product_version': 'Legion Go S 8APU1'},
                                     cpu='AMD Ryzen Z1 Extreme', ram_gib=30, cards=[('0x1002', 4 * GIB)]), {},
     'legion_go_s', {'TOTK_WINDOW_SIZE': '1920x1200', 'TOTK_OUTPUT_RES': '1280x800', 'TOTK_OPERATION_MODE': 'handheld',
                     'TOTK_FRAME_GEN': '2x', 'TOTK_VK_PRESENT': 'mailbox'}),
    ('legion go s by cpu', dict(cpu='AMD Ryzen Z2 Go w/ Radeon 680M', ram_gib=14), {}, 'legion_go_s', {}),
    ('first legion go is not the s', dict(dmi={'product_family': 'Legion Go 8APU1', 'product_version': 'Legion Go 8APU1'},
                                          cpu='AMD Ryzen Z1 Extreme', ram_gib=16), {},
     'pc_small', {'TOTK_OUTPUT_RES': '1920x1080', 'TOTK_OPERATION_MODE': '(unset)'}),
    ('pc rx 6600 xt', dict(cpu='AMD Ryzen 5 5600X 6-Core Processor', ram_gib=31.3, cards=[('0x1002', 8 * GIB)]), {},
     'pc_small', {'TOTK_OUTPUT_RES': '1920x1080', 'TOTK_WINDOW_SIZE': '(unset)', 'TOTK_FPS_LIMIT': '(unset)'}),
    ('this fork\'s pc', dict(cpu='AMD Ryzen 7 5800X3D 8-Core Processor', ram_gib=15.5, cards=[('0x1002', 8 * GIB)]), {},
     'pc_small', {'TOTK_OUTPUT_RES': '1920x1080', 'TOTK_FRAME_GEN': '2x', 'TOTK_VK_PRESENT': 'mailbox'}),
    ('player turns frame generation off', dict(ram_gib=15.5), {'TOTK_FRAME_GEN': 'off'}, 'pc_small',
     {'TOTK_FRAME_GEN': 'off', 'TOTK_OUTPUT_RES': '1920x1080'}),
    ('pc with 16 gb ram and a large card', dict(cpu='AMD Ryzen 7 7700X', ram_gib=15.5, cards=[('0x10de', None)]), {},
     'pc_small', {'TOTK_OUTPUT_RES': '1920x1080'}),
    ('pc rx 7900 xtx', dict(cpu='AMD Ryzen 7 7800X3D', ram_gib=31, cards=[('0x1002', 512 << 20), ('0x1002', 24 * GIB)]),
     {}, 'desktop', {'TOTK_OUTPUT_RES': '(unset)'}),
    ('pc nvidia beside an amd igpu', dict(cpu='AMD Ryzen 7 7700X', ram_gib=32, cards=[('0x1002', 512 << 20), ('0x10de', None)]),
     {}, 'desktop', {'TOTK_OUTPUT_RES': '(unset)'}),
    ('no files at all', dict(), {}, 'desktop', {'TOTK_OUTPUT_RES': '(unset)'}),
    ('player turns it off', dict(cpu='AMD Custom APU 0405'), {'TOTK_DEVICE_PROFILE': 'desktop'}, 'desktop',
     {'TOTK_OUTPUT_RES': '(unset)'}),
    ('player picks one', dict(ram_gib=64), {'DKB_DEVICE_PROFILE': 'legion_go_s'}, 'legion_go_s', {'TOTK_FPS_LIMIT': '40'}),
    ('player value wins over the profile', dict(cpu='AMD Custom APU 0405'), {'TOTK_OUTPUT_RES': '1920x1200'}, 'deck',
     {'TOTK_OUTPUT_RES': '1920x1200', 'TOTK_FPS_LIMIT': '40'}),
]

failed = 0
with tempfile.TemporaryDirectory() as tmp:
    for i, (name, spec, env, want_profile, want) in enumerate(CASES):
        root = Path(tmp) / str(i)
        root.mkdir()
        machine(root, **spec)
        got = run(root, env)
        bad = [f'profile {got.get("profile")} != {want_profile}'] if got.get('profile') != want_profile else []
        bad += [f'{k}={got.get(k)} != {v}' for k, v in want.items() if got.get(k) != v]
        print(('FAIL' if bad else 'ok  ') + f' {name}' + (': ' + '; '.join(bad) if bad else ''))
        failed += bool(bad)
if failed:
    sys.exit(f'{failed} of {len(CASES)} cases failed')
print(f'PASS: {len(CASES)} machines')
