"""installer/device.py: the start screen's line naming the machine and its settings. Which profile each machine gets
is checked against runtime/cfg.c by tests/device_profile_test.py.

Usage: python -m pytest tests/installer_device_test.py
"""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from installer import device
from installer.guilogic import Catalog


def machine(root, cpu, ram_gib, vram_gib):
    (root / 'proc').mkdir(parents=True)
    (root / 'proc/cpuinfo').write_text(f'processor\t: 0\nmodel name\t: {cpu}\n\n')
    (root / 'proc/meminfo').write_text(f'MemTotal:       {int(ram_gib * 1048576)} kB\n')
    d = root / 'sys/class/drm/card0/device'
    d.mkdir(parents=True)
    (d / 'vendor').write_text('0x1002\n')
    (d / 'mem_info_vram_total').write_text(f'{vram_gib << 30}\n')


def test_short_cpu():
    assert device.short_cpu('AMD Ryzen 7 5800X3D 8-Core Processor') == 'Ryzen 7 5800X3D'
    assert device.short_cpu('AMD Ryzen Z2 Go w/ Radeon 680M') == 'Ryzen Z2 Go'
    assert device.short_cpu('Intel(R) Core(TM) i7-12700K') == 'Core i7-12700K'


def test_this_forks_pc_in_german(tmp_path):
    machine(tmp_path, 'AMD Ryzen 7 5800X3D 8-Core Processor', 15.5, 8)
    found = device.detect(env={'DKB_DEVICE_SYSROOT': str(tmp_path)})
    assert found['profile'] == 'pc_small' and not found['set']
    cat = Catalog('de')
    text = device.detected_text(found, cat)
    assert text.startswith('Erkannt: PC (Ryzen 7 5800X3D, 8 GB Grafikspeicher, 16 GB RAM)')
    assert 'FSR 4' in text


def test_ini_choice_wins(tmp_path):
    machine(tmp_path, 'AMD Ryzen 7 5800X3D 8-Core Processor', 15.5, 8)
    dest = tmp_path / 'game'
    dest.mkdir()
    (dest / 'totk.ini').write_text('[display]\n# device_profile = auto\ndevice_profile = legion_go_s\n')
    found = device.detect(dest, env={'DKB_DEVICE_SYSROOT': str(tmp_path)})
    assert found['profile'] == 'legion_go_s' and found['set']
    assert device.detected_text(found, Catalog('en')).startswith('Device profile from totk.ini: Lenovo Legion Go S')


def test_every_language_has_every_text():
    for lang in ('en', 'de', 'ru'):
        cat = Catalog(lang)
        for profile in device.PROFILES:
            for id in ('ui.device_detected', 'ui.device_settings_' + profile, 'ui.device_change'):
                assert cat(id) != id, (lang, id)
