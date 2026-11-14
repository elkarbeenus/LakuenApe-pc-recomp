"""The setup window as the launcher (installer/launch.py): the remembered game, Play, and the AppImage update with its
checksum, without the network or a game."""
import hashlib
import io
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from installer import launch, update  # noqa: E402

NAME = 'LakuenApeRecomp-0.3.0.3+abc1234-x86_64.AppImage'
DATA = b'\x7fELF new appimage' * 1000


class Response(io.BytesIO):
    def __init__(self, data):
        super().__init__(data)
        self.headers = {'Content-Length': str(len(data))}

    def __enter__(self):
        return self

    def __exit__(self, *a):
        self.close()


def release(sums=None):
    sums = sums if sums is not None else f'{hashlib.sha256(DATA).hexdigest()}  {NAME}\n'
    return ({'tag_name': 'v0.3.0.3', 'html_url': 'https://example.invalid/r', 'assets': [
        {'name': NAME, 'browser_download_url': 'https://example.invalid/image'},
        {'name': 'SHA256SUMS.txt', 'browser_download_url': 'https://example.invalid/sums'}]},
            {'https://example.invalid/image': DATA, 'https://example.invalid/sums': sums.encode()})


class LaunchTest(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        self.settings = self.tmp / 'cfg' / 'setup.json'
        self.dest = self.tmp / 'game'
        self.dest.mkdir()

    def make_game(self):
        exe = self.dest / launch.GAME_EXE
        exe.write_text('#!/bin/sh\n')
        exe.chmod(0o755)
        return exe

    def test_remembers_only_a_built_game(self):
        self.assertIsNone(launch.remembered_game(self.settings))
        self.assertTrue(launch.remember(self.dest, self.settings))
        self.assertIsNone(launch.remembered_game(self.settings))   # no game in the folder yet
        self.make_game()
        self.assertEqual(launch.remembered_game(self.settings), self.dest.resolve())

    def test_play_starts_the_game_in_its_folder(self):
        with self.assertRaises(FileNotFoundError):
            launch.play(self.dest, popen=lambda *a, **k: 1 / 0)
        exe = self.make_game()
        seen = {}
        launch.play(self.dest, popen=lambda cmd, **kw: seen.update(cmd=cmd, **kw))
        self.assertEqual(seen['cmd'], [str(exe)])
        self.assertEqual(seen['cwd'], str(self.dest))

    def test_download_checks_the_checksum(self):
        info, files = release()
        got = []
        path = launch.download_update(info, self.tmp, lambda d, t: got.append((d, t)),
                                      opener=lambda url: Response(files[url]))
        self.assertEqual(path, self.tmp / NAME)
        self.assertEqual(path.read_bytes(), DATA)
        self.assertTrue(os.access(path, os.X_OK))
        self.assertEqual(got[-1], (len(DATA), len(DATA)))

    def test_bad_download_keeps_nothing(self):
        info, files = release(sums=f'{"0" * 64}  {NAME}\n')
        with self.assertRaises(ValueError):
            launch.download_update(info, self.tmp, opener=lambda url: Response(files[url]))
        self.assertEqual([p.name for p in self.tmp.iterdir() if p.is_file()], [])
        info, files = release(sums='')
        with self.assertRaises(ValueError):
            launch.download_update(info, self.tmp, opener=lambda url: Response(files[url]))
        with self.assertRaises(ValueError):
            launch.download_update({'assets': []}, self.tmp, opener=lambda url: 1 / 0)

    def test_update_starts_the_new_appimage_on_the_install(self):
        seen = {}
        with patch.dict(os.environ, {'APPIMAGE': '/old.AppImage', 'APPDIR': '/tmp/.mount'}):
            launch.start_updated(self.tmp / NAME, self.dest, popen=lambda cmd, **kw: seen.update(cmd=cmd, **kw))
        self.assertEqual(seen['cmd'], [str(self.tmp / NAME), '--dest', str(self.dest), '--auto-start'])
        self.assertNotIn('APPIMAGE', seen['env'])
        self.assertEqual(launch.update_folder(self.tmp / 'x.AppImage'), self.tmp)

    def test_installed_release_and_outdated(self):
        self.assertIsNone(launch.installed_release(self.dest))   # no state: not an install
        self.assertFalse(launch.outdated(self.dest, '0.3.0.6+abc'))
        state = self.dest / 'dkb-install.json'
        state.write_text('{"format": 1, "steps": {}}')
        self.assertEqual(launch.installed_release(self.dest), '')   # made before releases were recorded
        self.assertTrue(launch.outdated(self.dest, '0.3.0.6+abc'))
        state.write_text('{"format": 1, "steps": {}, "release": "0.3.0.6+abc"}')
        self.assertFalse(launch.outdated(self.dest, '0.3.0.6+abc'))
        self.assertTrue(launch.outdated(self.dest, '0.3.0.7+def'))

    def test_playtime_adds_up(self):
        self.assertEqual(launch.playtime(self.dest, self.settings), 0)
        launch.add_playtime(self.dest, 3600 + 25 * 60 + 30, self.settings)
        launch.add_playtime(self.dest, 120, self.settings)
        self.assertEqual(launch.hours_minutes(launch.playtime(self.dest, self.settings)), (1, 27))
        launch.remember(self.dest, self.settings)   # remembering keeps the play time
        self.assertEqual(launch.hours_minutes(launch.playtime(self.dest, self.settings)), (1, 27))
        self.assertEqual(launch.hours_minutes(59), (0, 0))

    def test_newer_release_info(self):
        info, _ = release()
        self.assertEqual(update.newer_release_info('0.3.0.2+abc', fetch=lambda: info)['tag_name'], 'v0.3.0.3')
        self.assertIsNone(update.newer_release_info('0.3.0.3+abc', fetch=lambda: info))
        self.assertIsNone(update.newer_release_info('0.3.0.2', fetch=lambda: 1 / 0))


if __name__ == '__main__':
    unittest.main()
