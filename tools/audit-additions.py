"""Verify the user's additions-only constraint and inventory every changed file."""
import hashlib
import pathlib
import subprocess

root = pathlib.Path(__file__).resolve().parents[1]


def git(*args):
    return subprocess.check_output(['git', '-C', str(root), *args])


changed = git('diff', '--name-only', 'HEAD', '-z').decode().split('\0')
new = git('ls-files', '--others', '--exclude-standard', '-z').decode().split('\0')
errors = []
for name in sorted(set(changed + new) - {''}):
    path = root / name
    if not path.is_file():
        errors.append(f'{name}: original file missing')
        continue
    data = path.read_bytes()
    if name in changed:
        original = git('show', f'HEAD:{name}').decode('utf-8').splitlines()
        current = iter(data.decode('utf-8').splitlines())
        for line in original:
            if not any(candidate == line for candidate in current):
                errors.append(f'{name}: an original line was removed or replaced')
                break
    print(f'{hashlib.sha256(data).hexdigest()}  {name}')
subprocess.run(['git', '-C', str(root), 'diff', '--check'], check=True)
if errors:
    raise SystemExit('\n'.join(errors))
print('PASS: all original lines preserved in order; every changed/new file inventoried.')
