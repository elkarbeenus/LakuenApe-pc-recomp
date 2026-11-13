"""What the window does to the computer when an install ends, for a build left running overnight or while away: nothing,
standby (suspend) or shut down. The window chooses, counts down a minute with a way to stop it, then calls run().

Linux asks systemd (systemctl, else loginctl), which lets the logged-in user of the active session suspend or power off
without a password (SteamOS, KDE and GNOME desktops). Windows suspends through powrprof's SetSuspendState (which
hibernates instead when hibernation is on) and shuts down with shutdown.exe.
"""
import os
import subprocess

ACTIONS = ('none', 'suspend', 'poweroff')
COUNTDOWN_S = 60


def commands(action, nt=None):
    """The commands that carry out action, tried in order until one succeeds; [] for 'none'."""
    if action not in ACTIONS:
        raise ValueError(action)
    if action == 'none':
        return []
    nt = os.name == 'nt' if nt is None else nt
    if nt:
        return [['rundll32.exe', 'powrprof.dll,SetSuspendState', '0,1,0']] if action == 'suspend' else \
               [['shutdown.exe', '/s', '/t', '0']]
    return [['systemctl', action], ['loginctl', action]]


def applies(action, result):
    """Whether action runs after an install that ended with result ('ok', 'failed', 'cancelled'): a cancel means the
    player is at the computer, so it never runs then."""
    return action != 'none' and result in ('ok', 'failed')


def run(action, runner=subprocess.run):
    """Carries out action; True when a command accepted it. Never raises."""
    for command in commands(action):
        try:
            if runner(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=30).returncode == 0:
                return True
        except (OSError, subprocess.SubprocessError):
            continue
    return False
