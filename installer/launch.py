"""The setup window as the game's launcher: the install folder it remembers, Play, and the update of the AppImage itself.

After a finished install the window remembers the folder (settings_path(), outside the read-only AppImage). Started
again, it opens on that game: Play starts it, and when this fork has a newer release (installer/update.py) the window
asks whether to update first or to play the current version. An update downloads the release's AppImage next to the
one running ($APPIMAGE), checks it against the release's SHA256SUMS.txt, and starts it on the same install folder,
where the installer updates the recorded install. No Tk here: the window draws, this does the work.
"""
import hashlib
import json
import os
import re
import subprocess
import tempfile
import urllib.request
from pathlib import Path

from . import state as statefile
from . import update

GAME_EXE = 'dkb.exe' if os.name == 'nt' else 'dkb'
ASSET = re.compile(r'LakuenApeRecomp-.+-x86_64\.AppImage')
SUMS = 'SHA256SUMS.txt'
TIMEOUT = 30


def settings_path():
    base = os.environ.get('XDG_CONFIG_HOME') or os.path.join(os.path.expanduser('~'), '.config')
    if os.name == 'nt':
        base = os.environ.get('APPDATA') or base
    return Path(base) / 'lakuenape' / 'setup.json'


def load_settings(path=None):
    try:
        return json.loads((path or settings_path()).read_text(encoding='utf-8'))
    except (OSError, ValueError):
        return {}


def remember(dest, path=None):
    """Keeps dest as the game the next start opens on. Never raises: a launcher without memory still installs."""
    path = path or settings_path()
    try:
        data = load_settings(path)
        data['dest'] = str(Path(dest).resolve())
        path.parent.mkdir(parents=True, exist_ok=True)
        tmp = path.with_suffix('.tmp')
        tmp.write_text(json.dumps(data, indent=1), encoding='utf-8')
        os.replace(tmp, path)
        return True
    except OSError:
        return False


def game_in(dest):
    """The built game in dest, or None."""
    exe = Path(dest) / GAME_EXE if dest else None
    return exe if exe and exe.is_file() and (os.name == 'nt' or os.access(exe, os.X_OK)) else None


def remembered_game(path=None):
    """The remembered install folder when its game is still there, else None."""
    dest = load_settings(path).get('dest')
    return Path(dest) if dest and game_in(dest) else None


def installed_release(dest):
    """The release that last finished an install in dest (dkb-install.json, installer/bananza.py main), '' for an
    install made before releases were recorded, or None without a readable state."""
    try:
        state = statefile.load(Path(dest), 'dkb-install.json')
    except (statefile.StateError, OSError):
        return None
    return None if state is None else str(state.get('release') or '')


def outdated(dest, release):
    """Whether the game in dest was built by an older release than the one running (or by one that recorded none):
    the setup updates it then. A game built by a newer setup is not outdated; the update check offers that setup."""
    installed = installed_release(dest)
    if installed is None:
        return False
    return installed == '' or update.compare(installed, release) == -1


def plain(version):
    """'0.4.2.9' from '0.4.2.9+88a4ff6': the version as the player reads it, without the commit."""
    return str(version).split('+', 1)[0]


def playtime(dest, path=None):
    """Seconds played from dest through this window (Play, --play)."""
    try:
        return int(load_settings(path).get('playtime', {}).get(str(Path(dest).resolve()), 0))
    except (TypeError, ValueError):
        return 0


def add_playtime(dest, seconds, path=None):
    """Adds a session's seconds to dest's play time. Never raises."""
    path = path or settings_path()
    try:
        data = load_settings(path)
        times = data.setdefault('playtime', {})
        key = str(Path(dest).resolve())
        times[key] = int(times.get(key, 0)) + max(0, int(seconds))
        path.parent.mkdir(parents=True, exist_ok=True)
        tmp = path.with_suffix('.tmp')
        tmp.write_text(json.dumps(data, indent=1), encoding='utf-8')
        os.replace(tmp, path)
        return True
    except (OSError, TypeError, ValueError):
        return False


def hours_minutes(seconds):
    """(hours, minutes) of a play time, minutes rounded down."""
    minutes = max(0, int(seconds)) // 60
    return minutes // 60, minutes % 60


def play(dest, popen=subprocess.Popen):
    """Starts the game from its folder; the process (the window waits for it, so Steam sees the game run)."""
    exe = game_in(dest)
    if exe is None:
        raise FileNotFoundError(str(Path(dest) / GAME_EXE))
    return popen([str(exe)], cwd=str(exe.parent), start_new_session=os.name != 'nt')


def running_appimage():
    """The AppImage this window runs from ($APPIMAGE, which the AppImage runtime sets), or None."""
    p = os.environ.get('APPIMAGE')
    return Path(p) if p and os.name != 'nt' and Path(p).is_file() else None


def release_assets(info):
    """(AppImage url, its name, SHA256SUMS.txt url) of a release's JSON, or None when it lacks either."""
    image = sums = None
    for a in info.get('assets') or ():
        if ASSET.fullmatch(a.get('name', '')):
            image = (a['browser_download_url'], a['name'])
        elif a.get('name') == SUMS:
            sums = a['browser_download_url']
    return (image[0], image[1], sums) if image and sums else None


def expected_sha(sums_text, name):
    for line in sums_text.splitlines():
        parts = line.split()
        if len(parts) == 2 and parts[1].lstrip('*') == name:
            return parts[0].lower()
    return None


def fetch(url, timeout=TIMEOUT):
    req = urllib.request.Request(url, headers={'User-Agent': 'lakuenape-setup'})
    return urllib.request.urlopen(req, timeout=timeout)


def download_update(info, folder, progress=lambda done, total: None, opener=fetch):
    """The release's AppImage, downloaded into folder, checked against SHA256SUMS.txt and made executable: its path.
    Raises OSError or ValueError with a reason the window shows; a half download never keeps the final name."""
    found = release_assets(info)
    if not found:
        raise ValueError('the release has no AppImage with checksums')
    url, name, sums_url = found
    with opener(sums_url) as r:
        want = expected_sha(r.read().decode('utf-8', 'replace'), name)
    if not want:
        raise ValueError(f'{SUMS} lists no {name}')
    folder = Path(folder)
    target = folder / name
    fd, part = tempfile.mkstemp(prefix='.' + name + '.', suffix='.part', dir=folder)
    try:
        h = hashlib.sha256()
        with os.fdopen(fd, 'wb') as out, opener(url) as r:
            total = int(r.headers.get('Content-Length') or 0)
            done = 0
            while True:
                block = r.read(1 << 20)
                if not block:
                    break
                out.write(block)
                h.update(block)
                done += len(block)
                progress(done, total)
        if h.hexdigest() != want:
            raise ValueError('the download does not match its checksum')
        os.chmod(part, 0o755)
        os.replace(part, target)
        return target
    finally:
        if os.path.exists(part):
            os.remove(part)


def update_folder(appimage):
    """Where the new AppImage goes: beside the running one when that folder is writable, else ~/Applications."""
    if appimage and os.access(appimage.parent, os.W_OK):
        return appimage.parent
    folder = Path(os.path.expanduser('~')) / 'Applications'
    folder.mkdir(parents=True, exist_ok=True)
    return folder


def start_updated(image, dest, popen=subprocess.Popen):
    """Starts the downloaded AppImage on the install folder; it updates the install there by itself (--auto-start)."""
    env = {k: v for k, v in os.environ.items() if k not in ('APPIMAGE', 'APPDIR', 'ARGV0', 'OWD')}
    return popen([str(image), '--dest', str(dest), '--auto-start'], env=env, start_new_session=True)
