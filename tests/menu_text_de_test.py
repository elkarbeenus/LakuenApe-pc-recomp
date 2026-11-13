"""The settings menu in German (runtime/ui_text.c text_de): every English text has a German one with the same printf
conversions, and a German system language picks it, without the game or a GPU.

Usage: python tests/menu_text_de_test.py [--cc /path/to/clang]
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

src = Path(__file__).resolve().parents[1]
ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument('--cc', default=os.environ.get('CC') or shutil.which('clang'))
args = ap.parse_args()
if not args.cc:
    ap.error('pass --cc with the project Clang executable')
out = src / 'build' / 'tests' / 'menu-text-de'
out.mkdir(parents=True, exist_ok=True)
(out / 'check.c').write_text(r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_text.h"
const char *cfg_value(const char *n) { (void)n; return getenv("MENU_LANG"); }
unsigned cfg_version(void) { return 0; }
const char *ov_language(void) { return "en"; }
const char *plat_ui_language(void) { return getenv("SYS_LANG"); }
// printf conversions as the menu formats them: '%' then flags without a space, width, precision and a letter (or %%)
static void specs(const char *s, char *out, size_t cap) {
    out[0] = 0;
    for (; *s; s++) {
        if (*s != '%') continue;
        size_t n = strspn(s + 1, "-+#0123456789.");
        char c = s[1 + n];
        if (c == '%' || (c >= 'a' && c <= 'z' && c != ' ')) {
            strncat(out, s, n + 2 < cap - strlen(out) - 1 ? n + 2 : 0);
            s += n + 1;
        }
    }
}
int main(int argc, char **argv) {
    if (argc > 1) { puts(menu_text(UI_MENU_TITLE)); return 0; }
    int bad = 0;
    for (int i = 0; i < UI_TEXT_COUNT; i++) {
        if (!ui_text_defined("en", (UiTextId)i)) continue;
        if (!ui_text_defined("de", (UiTextId)i)) { printf("id %d has no German text\n", i); bad++; continue; }
        char a[128], b[128];
        specs(ui_text_in("en", (UiTextId)i), a, sizeof a);
        specs(ui_text_in("de", (UiTextId)i), b, sizeof b);
        if (strcmp(a, b)) { printf("id %d: English '%s', German '%s'\n", i, a, b); bad++; }
    }
    return bad != 0;
}
''')
exe = out / 'check'
r = subprocess.run([args.cc, '-O1', '-w', '-I', str(src / 'runtime'), str(out / 'check.c'), str(src / 'runtime' / 'ui_text.c'),
                    '-o', str(exe)], capture_output=True, text=True)
if r.returncode:
    sys.exit(f'compile failed:\n{r.stderr[-3000:]}')
r = subprocess.run([str(exe)], capture_output=True, text=True, timeout=20)
if r.returncode:
    sys.exit('German menu text:\n' + r.stdout)


def title(sys_lang, menu_lang=None):
    env = {k: v for k, v in os.environ.items() if k not in ('MENU_LANG', 'SYS_LANG')}
    env['SYS_LANG'] = sys_lang
    if menu_lang:
        env['MENU_LANG'] = menu_lang
    return subprocess.run([str(exe), 'title'], env=env, capture_output=True, text=True, timeout=20).stdout.strip()


cases = [(('de-DE',), 'Einstellungen'), (('de',), 'Einstellungen'), (('en-US',), 'Settings'), (('ru-RU',), 'Настройки'),
         (('ru-RU', 'de'), 'Einstellungen'), (('de-AT', 'en'), 'Settings')]
failed = [(a, want, title(*a)) for a, want in cases if title(*a) != want]
if failed:
    sys.exit(f'menu language: {failed}')
print(f'PASS: every menu text in German with the same formats; {len(cases)} language choices')
