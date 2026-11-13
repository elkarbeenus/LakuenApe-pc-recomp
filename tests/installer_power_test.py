"""The window's when-done actions (installer/power.py), the build's lower priority (installer/bananza.py
lower_priority) and the desktop file chooser (installer/guilogic.py native_pick), without changing the computer."""
import os
from pathlib import Path
import subprocess
import sys
from types import SimpleNamespace
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from installer import bananza, power  # noqa: E402
from installer.guilogic import NO_NATIVE, native_pick  # noqa: E402


class PowerTest(unittest.TestCase):
    def test_commands(self):
        self.assertEqual(power.commands('none', nt=False), [])
        self.assertEqual(power.commands('suspend', nt=False), [['systemctl', 'suspend'], ['loginctl', 'suspend']])
        self.assertEqual(power.commands('poweroff', nt=False)[0], ['systemctl', 'poweroff'])
        self.assertEqual(power.commands('poweroff', nt=True), [['shutdown.exe', '/s', '/t', '0']])
        self.assertIn('SetSuspendState', ' '.join(power.commands('suspend', nt=True)[0]))
        with self.assertRaises(ValueError):
            power.commands('reboot')

    def test_never_after_a_cancel(self):
        self.assertTrue(power.applies('suspend', 'ok'))
        self.assertTrue(power.applies('poweroff', 'failed'))   # a build left overnight that failed still ends
        self.assertFalse(power.applies('suspend', 'cancelled'))
        self.assertFalse(power.applies('none', 'ok'))

    def test_run_falls_back_and_never_raises(self):
        tried = []

        def runner(command, **_kw):
            tried.append(command[0])
            if command[0] == 'systemctl':
                raise FileNotFoundError(command[0])
            return SimpleNamespace(returncode=0)
        with patch.object(power.os, 'name', 'posix'):
            self.assertTrue(power.run('suspend', runner=runner))
        self.assertEqual(tried, ['systemctl', 'loginctl'])
        with patch.object(power.os, 'name', 'posix'):
            self.assertFalse(power.run('poweroff', runner=lambda c, **k: SimpleNamespace(returncode=1)))
        self.assertTrue(power.run('none', runner=lambda c, **k: 1 / 0) is False)


class PriorityTest(unittest.TestCase):
    @unittest.skipIf(os.name == 'nt', 'Linux only')
    def test_lowered_and_switch(self):
        calls = []
        with patch.dict(os.environ, {'DKB_NICE': ''}):
            got = bananza.lower_priority(nice=calls.append, ioprio=lambda v: calls.append(v) or 0)
        self.assertEqual(got, ['cpu', 'disk'])
        self.assertEqual(calls, [10, (2 << 13) | 7])   # best effort, level 7
        with patch.dict(os.environ, {'DKB_NICE': '0'}):
            self.assertEqual(bananza.lower_priority(nice=lambda n: 1 / 0, ioprio=lambda v: 1 / 0), [])


class NativePickTest(unittest.TestCase):
    def pick(self, have, code=0, out='/home/deck/Games/game.nup\n', **kw):
        seen = []

        def run(command, **_kw):
            seen.append(command)
            return SimpleNamespace(returncode=code, stdout=out)
        with patch.object(os, 'name', 'posix'), patch.dict(os.environ, {'DKB_TK_DIALOGS': ''}):
            got = native_pick('Pick', which=lambda n: f'/usr/bin/{n}' if n in have else None, run=run, **kw)
        return got, seen

    def test_kdialog_first(self):
        got, seen = self.pick({'kdialog', 'zenity'}, types=[('Nintendo .nup', '*.nup'), ('All', '*.*')])
        self.assertEqual(got, '/home/deck/Games/game.nup')
        self.assertEqual(seen[0][:4], ['kdialog', '--title', 'Pick', '--getopenfilename'])
        self.assertEqual(seen[0][-1], '*.nup|Nintendo .nup')

    def test_zenity_folder_and_cancel(self):
        got, seen = self.pick({'zenity'}, code=1, out='', folder=True)
        self.assertEqual(got, '')   # cancelled: no Tk chooser after it
        self.assertIn('--directory', seen[0])

    def test_falls_back_to_tk(self):
        self.assertIs(self.pick(set())[0], NO_NATIVE)
        self.assertIs(self.pick({'kdialog'}, code=254, out='')[0], NO_NATIVE)
        with patch.dict(os.environ, {'DKB_TK_DIALOGS': '1'}):
            self.assertIs(native_pick('Pick', which=lambda n: '/usr/bin/' + n, run=lambda *a, **k: 1 / 0), NO_NATIVE)


if __name__ == '__main__':
    unittest.main()
