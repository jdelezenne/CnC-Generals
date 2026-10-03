"""Regenerate the checked-in manifests from EA's original VC6 game projects.

Python is needed only when updating the manifests, never to configure or build.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parent.parent
PROJECTS = {
    "wwdebug": "Libraries/Source/WWVegas/WWDebug/wwdebug.dsp",
    "wwlib": "Libraries/Source/WWVegas/WWLib/wwlib.dsp",
    "wwmath": "Libraries/Source/WWVegas/WWMath/wwmath.dsp",
    "wwutil": "Libraries/Source/WWVegas/Wwutil/wwutil.dsp",
    "wwsaveload": "Libraries/Source/WWVegas/WWSaveLoad/wwsaveload.dsp",
    "ww3d2": "Libraries/Source/WWVegas/WW3D2/ww3d2.dsp",
    "wwdownload": "Libraries/Source/WWVegas/WWDownload/WWDownload.dsp",
    "compression": "Libraries/Source/Compression/Compression.dsp",
    "gameengine": "GameEngine/GameEngine.dsp",
    "gameenginedevice": "GameEngineDevice/GameEngineDevice.dsp",
    "generals": "RTS.dsp",
}


def quoted(value):
    return '"' + value.replace('"', '\\"') + '"'


def relative(path, parent):
    return (parent / path.replace("\\", "/")).resolve().relative_to(ROOT).as_posix()


def selected(lines, config):
    stack = []
    active = True
    for line in lines:
        if line.startswith("!IF ") or line.startswith("!ELSEIF "):
            match = re.search(r'"\$\(CFG\)" == "([^"]+)"', line)
            if not match:
                raise ValueError(f"Unsupported DSP condition: {line}")
            matches = match[1].endswith(" - Win32 " + config)
            if line.startswith("!IF "):
                stack.append([active, matches])
                active = active and matches
            else:
                parent, taken = stack[-1]
                active = parent and not taken and matches
                stack[-1][1] = taken or matches
        elif line.startswith("!ELSE"):
            parent, taken = stack[-1]
            active = parent and not taken
            stack[-1][1] = True
        elif line.startswith("!ENDIF"):
            active = stack.pop()[0]
        elif active:
            yield line


def extract(path, config):
    lines = list(selected(path.read_text(encoding="latin1").splitlines(), config))
    flags = next(line[len("# ADD CPP "):] for line in lines if line.startswith("# ADD CPP "))
    includes = [relative(m[1], path.parent) for m in re.finditer(r'/I\s+"([^"]+)"', flags)]
    defines = [m[1] or m[2] for m in re.finditer(r'/D\s+(?:"([^"]+)"|(\S+))', flags)]
    # /YX, /Yu, /Yc and /Fr are IDE/PCH/browser bookkeeping. CMake compiles
    # translation units independently. Preserve code generation and warnings.
    stripped = re.sub(r'/[ID]\s+(?:"[^"]+"|\S+)', '', flags)
    options = [f for f in stripped.split() if f in {
        '/G6', '/W3', '/WX', '/GX', '/Gi', '/GR-', '/O2', '/Od', '/Ob2', '/Op', '/GZ',
        '/Ot', '/Og', '/Oi', '/Oy', '/Gy', '/GF', '/Gs'
    }]
    sources, overrides, shaders, headers = [], [], [], []
    block = []
    for line in lines:
        if line == "# Begin Source File":
            block = []
        elif line == "# End Source File":
            source = next((l[7:].strip().strip('"') for l in block if l.startswith('SOURCE=')), None)
            if not source:
                continue
            if '# PROP Exclude_From_Build 1' in block:
                continue
            suffix = Path(source).suffix.lower()
            source = relative(source, path.parent)
            if suffix in {'.vsh', '.psh'}:
                shaders.append(source)
                continue
            if suffix == '.h':
                headers.append(source)
                continue
            if suffix not in {'.cpp', '.c', '.rc'}:
                continue
            sources.append(source)
            for item in block:
                if item.startswith('# ADD CPP '):
                    extra = [f for f in item[10:].split() if f in {'/GX', '/GX-', '/Od', '/O2', '/G6', '/Op'}]
                    if extra:
                        overrides.append((source, extra))
        else:
            block.append(line)
    return sources, includes, defines, options, overrides, shaders, headers


def generate(tree):
    output = ['# Generated from EA\'s DSPs by cmake/extract-dsp.py. Do not edit.', '']
    projects = dict(PROJECTS)
    if tree == 'GeneralsMD':
        projects['profile'] = 'Libraries/Source/profile/profile.dsp'
        projects['eadebug'] = 'Libraries/Source/debug/debug.dsp'
        projects['wwshade'] = 'Libraries/Source/WWVegas/wwshade/wwshade.dsp'
    for name, dsp in projects.items():
        path = ROOT / tree / 'Code' / dsp
        output.append(f'# {tree}/Code/{dsp}')
        for config in ('Debug', 'Release'):
            sources, includes, defines, options, overrides, shaders, headers = extract(path, config)
            output.append(f'if(CMAKE_BUILD_TYPE STREQUAL "{config}")')
            groups = [('SOURCES', sources), ('INCLUDES', includes), ('DEFINES', defines), ('OPTIONS', options)]
            if name == 'wwshade':
                groups.extend([('SHADERS', shaders), ('HEADERS', headers)])
            for suffix, values in groups:
                output.append(f'    set(GEN_{name}_{suffix}')
                for value in dict.fromkeys(values):
                    if suffix in ('SOURCES', 'INCLUDES', 'SHADERS', 'HEADERS'):
                        value = '${PROJECT_SOURCE_DIR}/' + value
                    output.append('        ' + quoted(value))
                output.append('    )')
            for source, extra in overrides:
                output.append(f'    set_property(SOURCE "${{PROJECT_SOURCE_DIR}}/{source}" APPEND PROPERTY COMPILE_OPTIONS {" ".join(extra)})')
            output.append('endif()')
        output.append('')
    (ROOT / 'cmake' / f'{tree}Sources.cmake').write_text('\n'.join(output).rstrip('\n') + '\n', encoding='utf8')


if __name__ == '__main__':
    for tree in ('Generals', 'GeneralsMD'):
        generate(tree)
