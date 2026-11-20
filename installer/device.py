"""The machine the launcher runs on, as the game will see it: the device profile runtime/cfg.c picks at every start
("device profiles": profile_of, on_steam_deck, on_legion_go_s, on_small_pc), so the start screen can say which
machine it recognised and which settings the game will start with.

The rules here follow cfg.c and read the same files (/sys/class/dmi/id, /proc/cpuinfo, /proc/meminfo, amdgpu's
mem_info_vram_total), below DKB_DEVICE_SYSROOT when that is set. tests/device_profile_test.py runs both on the same
machines and fails when they disagree.
"""
import os
from pathlib import Path
import re

PROFILES = ('desktop', 'deck', 'legion_go_s', 'pc_small')
RAM_SMALL_KB = 17301504           # 16.5 GiB, cfg.c on_small_pc
VRAM_SMALL = 9126805504           # 8.5 GiB
INI_KEY = re.compile(r'^\s*device_profile\s*=\s*(\S+)', re.I)


def _path(root, path):
    return Path((root or '') + path)


def _line(root, path):
    try:
        with open(_path(root, path), encoding='utf-8', errors='replace') as f:
            return f.readline().strip()
    except OSError:
        return None


def cpu_name(root=None):
    """/proc/cpuinfo's first "model name", or ''."""
    try:
        text = _path(root, '/proc/cpuinfo').read_text(encoding='utf-8', errors='replace')
    except OSError:
        return ''
    m = re.search(r'^model name\s*:\s*(.+)$', text, re.M)
    return m.group(1).strip() if m else ''


def ram_kb(root=None):
    try:
        text = _path(root, '/proc/meminfo').read_text(encoding='utf-8', errors='replace')
    except OSError:
        return 0
    m = re.search(r'^MemTotal:\s*(\d+) kB', text, re.M)
    return int(m.group(1)) if m else 0


def cards(root=None):
    """The graphics cards as (vendor id, video memory in bytes or 0), in cfg.c's order (card0 to card15)."""
    out = []
    for i in range(16):
        vendor = _line(root, f'/sys/class/drm/card{i}/device/vendor')
        if vendor is None:
            continue
        size = _line(root, f'/sys/class/drm/card{i}/device/mem_info_vram_total') if vendor == '0x1002' else None
        out.append((vendor, int(size) if size and size.isdigit() else 0))
    return out


def on_steam_deck(cpu, root=None, env=os.environ):
    if env.get('SteamDeck') == '1':
        return True
    vendor = _line(root, '/sys/class/dmi/id/board_vendor') or _line(root, '/sys/class/dmi/id/sys_vendor') or ''
    if vendor.startswith('Valve') and _line(root, '/sys/class/dmi/id/product_name') in ('Jupiter', 'Galileo'):
        return True
    return any(apu in cpu.lower() for apu in ('custom apu 0405', 'custom apu 0932'))


def on_legion_go_s(cpu, root=None):
    for field in ('product_family', 'product_version', 'product_name'):
        if 'legion go s' in (_line(root, f'/sys/class/dmi/id/{field}') or '').lower():
            return True
    return 'ryzen z2 go' in cpu.lower()


def on_small_pc(root=None):
    ram = ram_kb(root)
    if ram and ram <= RAM_SMALL_KB:
        return True
    found = cards(root)
    vram = max((v for _, v in found), default=0)
    return bool(found) and all(v == '0x1002' for v, _ in found) and 0 < vram <= VRAM_SMALL


def chosen(dest=None, env=os.environ):
    """The profile the player set (TOTK_/DKB_DEVICE_PROFILE, else device_profile in dest's totk.ini), or None for
    auto. The environment wins over the file, as in cfg.c."""
    for name in ('TOTK_DEVICE_PROFILE', 'DKB_DEVICE_PROFILE'):
        if env.get(name):
            return env[name].lower()
    if dest:
        try:
            for line in (Path(dest) / 'totk.ini').read_text(encoding='utf-8', errors='replace').splitlines():
                m = INI_KEY.match(line)
                if m:
                    return m.group(1).lower()
        except OSError:
            pass
    return None


def detect(dest=None, env=None):
    """{'profile': one of PROFILES, 'set': whether the player chose it, 'cpu', 'ram_gib', 'vram_gib'}."""
    env = os.environ if env is None else env
    root = env.get('DKB_DEVICE_SYSROOT')
    cpu = cpu_name(root)
    set_ = chosen(dest, env)
    if set_ and set_ != 'auto':
        profile = set_ if set_ in PROFILES else 'desktop'
    else:
        set_ = None
        profile = ('deck' if on_steam_deck(cpu, root, env) else 'legion_go_s' if on_legion_go_s(cpu, root)
                   else 'pc_small' if on_small_pc(root) else 'desktop')
    vram = max((v for vendor, v in cards(root) if vendor == '0x1002'), default=0)
    return {'profile': profile, 'set': bool(set_), 'cpu': cpu, 'ram_gib': round(ram_kb(root) / 1048576),
            'vram_gib': round(vram / (1 << 30))}


def short_cpu(name):
    """'AMD Ryzen 7 5800X3D 8-Core Processor' -> 'Ryzen 7 5800X3D'."""
    name = re.sub(r'\(R\)|\(TM\)', '', name)
    name = re.split(r'\s+(?:\d+-Core\b|w/|with\b)', name)[0]
    name = re.sub(r'\s+Processor$', '', name).replace('AMD ', '').replace('Intel ', '')
    return ' '.join(name.split())


def describe(found, cat):
    """The start screen's line naming the machine, in the catalog's language."""
    profile = found['profile']
    if profile in ('deck', 'legion_go_s'):
        return cat('ui.device_' + profile)
    parts = [p for p in (short_cpu(found['cpu']),
                         cat('ui.device_vram', n=found['vram_gib']) if found['vram_gib'] else '',
                         cat('ui.device_ram', n=found['ram_gib']) if found['ram_gib'] else '') if p]
    return cat('ui.device_pc', parts=', '.join(parts)) if parts else cat('ui.device_pc_unknown')


def detected_text(found, cat):
    """Two lines: what was recognised, and what the game starts with there."""
    key = 'ui.device_chosen' if found['set'] else 'ui.device_detected'
    return cat(key, device=describe(found, cat)) + '\n' + cat('ui.device_settings_' + found['profile'])
