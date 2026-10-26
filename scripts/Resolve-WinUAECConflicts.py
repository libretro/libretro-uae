"""Resolve scratch merge regions that differ only in known C-port syntax."""

import argparse
import json
import pathlib
import re

CONFLICT = re.compile(r'^<<<<<<< PUAE\n(.*?)^\|\|\|\|\|\|\| WinUAE-base\n(.*?)^=======\n(.*?)^>>>>>>> WinUAE-target\n', re.M | re.S)


def canonical(text):
    tokens = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', re.S)
    text = tokens.sub(lambda match: '' if match.group().startswith(('/*', '//')) else match.group(), text)
    text = re.sub(r'\b(struct|enum)\s+', '', text)
    text = re.sub(r'\bsizeof\s*\(([^()]+)\)', r'sizeof \1', text)
    text = re.sub(r'\((int|float|double|bool|uae_[su](?:8|16|32|64))\)\s*\(', r'\1(', text)
    return re.sub(r'\s+', '', text)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--scratch', required=True)
    args = parser.parse_args()
    scratch = pathlib.Path(args.scratch)
    outcomes = json.loads((scratch / 'outcomes.json').read_text())
    resolutions = []
    remaining = []
    for item in outcomes:
        path = scratch / item['local']
        text = path.read_text(encoding='utf-8', errors='surrogateescape')
        count = 0

        def resolve(match):
            nonlocal count
            count += 1
            ours, base, theirs = match.groups()
            if canonical(ours) == canonical(base):
                resolutions.append({'local': item['local'], 'region': count, 'resolution': 'upstream; local changes were C syntax or formatting only'})
                return theirs
            if canonical(ours) == canonical(theirs):
                resolutions.append({'local': item['local'], 'region': count, 'resolution': 'local C form; both sides equivalent'})
                return ours
            return match.group(0)

        text = CONFLICT.sub(resolve, text)
        path.write_text(text, encoding='utf-8', errors='surrogateescape', newline='\n')
        if '<<<<<<< PUAE' in text:
            remaining.append({'local': item['local'], 'regions': text.count('<<<<<<< PUAE')})
    (scratch / 'syntax-resolutions.json').write_text(json.dumps(resolutions, indent=2) + '\n')
    (scratch / 'remaining.json').write_text(json.dumps(remaining, indent=2) + '\n')
    print(json.dumps({'resolved_regions': len(resolutions), 'remaining_files': len(remaining), 'remaining_regions': sum(i['regions'] for i in remaining), 'remaining': remaining}, indent=2))


if __name__ == '__main__':
    main()
