"""Regenerate typed offline definitions from VC6-preprocessed vendor headers.

Run cl /P on gamespy-disabled.cpp with cmake/stubs on the include path, then
pass the resulting .i file. This is a maintainer tool, not a build dependency.
"""
import re
import sys
from pathlib import Path

text = Path(sys.argv[1]).read_text(encoding='latin1')
text = re.sub(r'^#.*$', '', text, flags=re.M)
prototype = re.compile(r'^([A-Za-z_]\w*(?:[ \t]+\w+)*[ \t]*\**)[ \t\n]*\b(\w+)\s*\(([^;{}]*)\)\s*;', re.M)
functions = {}
for match in prototype.finditer(text):
    result, name, arguments = match.groups()
    if not re.match(r'^(peer|gp[A-Z]|ghttp|SBServer|qr2_|pt[A-Z]|InitStats|CloseStats|NewGame|FreeGame|SendGame|Bucket|Persist|GetPersist|SetPersist|PreAuth|GetChallenge|GenerateAuth|IsStats|StatsThink)', name):
        continue
    result = ' '.join(result.split())
    arguments = ' '.join(arguments.split())
    if result == 'void':
        body = ''
    elif '*' in result or result in ('PEER', 'SBServer', 'GServer', 'statsgame_t', 'bucketset_t'):
        body = 'return NULL;'
    elif result == 'GPResult':
        body = 'return GP_NETWORK_ERROR;'
    elif result.startswith('GHTTP') and result not in ('GHTTPBool',):
        body = f'return ({result})-1;'
    else:
        body = f'return ({result})0;'
    if name == 'gpInitialize':
        body = '*connection = NULL; return GP_NETWORK_ERROR;'
    elif name.startswith('InitStats') or name == 'StatsThink':
        body = 'return GE_NOCONNECT;'
    elif name == 'SBServerGetStringValueA':
        body = 'return def;'
    elif name == 'SBServerGetIntValueA':
        body = 'return idefault;'
    elif name == 'SBServerGetFloatValueA':
        body = 'return fdefault;'
    functions[name] = f'{result} {name}({arguments}) {{ {body} }}'
output = ['// Generated offline definitions. No networking implementation is linked.',
          '// Regenerate with cmake/stubs/generate-offline.py.', 'extern "C" {']
output += [functions[name] for name in sorted(functions)]
output += ['}', '']
Path(__file__).with_name('gamespy-functions.inc').write_text('\n'.join(output), encoding='utf8')
print(f'Generated {len(functions)} offline SDK definitions')
