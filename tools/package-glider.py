"""Package a built custom glider with its complete GPL source and verify each ZIP member."""
import argparse
import hashlib
import pathlib
import subprocess
import zipfile

root = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('destination', type=pathlib.Path)
args = parser.parse_args()
destination = args.destination.resolve()
destination.mkdir(parents=True, exist_ok=True)
stage = root / 'build/package/Spidy-0.4.3'
if not (stage / 'Spidy Launcher.exe').is_file():
    raise SystemExit('Run tools/package.ps1 -KeepFolder after building first.')
subprocess.run(['python', '-B', str(root / 'tools/audit-additions.py')], check=True)


def git(*options):
    return subprocess.check_output(['git', '-C', str(root), *options])


def archive(name, members):
    target = destination / name
    if target.exists():
        raise SystemExit(f'Existing output preserved: {target}')
    with zipfile.ZipFile(target, 'x', compression=zipfile.ZIP_DEFLATED) as output:
        for filename, path in sorted(members.items()):
            output.write(path, filename)
    with zipfile.ZipFile(target) as output:
        if output.testzip() is not None:
            raise SystemExit('ZIP CRC validation failed')
        for filename, path in members.items():
            if hashlib.sha256(output.read(filename)).digest() != hashlib.sha256(path.read_bytes()).digest():
                raise SystemExit(f'ZIP content mismatch: {filename}')
    print(f'VERIFIED {target.name}: {len(members)} files; SHA256 {hashlib.sha256(target.read_bytes()).hexdigest()}')


source_names = (git('ls-files', '-z') + git('ls-files', '--others', '--exclude-standard', '-z')).decode().split('\0')
source = {f'SpidyVR-Glider-Source/{name}': root / name for name in source_names if name}
archive('SpidyVR-Glider-Custom-Source.zip', source)
player = {f'SpidyVR-Glider-Custom/{p.relative_to(stage).as_posix()}': p for p in stage.rglob('*') if p.is_file()}
player['SpidyVR-Glider-Custom/build/windows-ninja/spidy-glider.ini'] = root / 'spidy-glider.ini'
player['SpidyVR-Glider-Custom/GLIDER.md'] = root / 'docs/GLIDER.md'
player['SpidyVR-Glider-Custom/README-GLIDER-TR.txt'] = destination / 'README-GLIDER-TR.txt'
archive('SpidyVR-Glider-Custom-Win64.zip', player)
patch = destination / 'glider-additions.patch'
if patch.exists():
    raise SystemExit(f'Existing output preserved: {patch}')
# The source ZIP also includes new files; the patch only contains tracked-file insertions.
patch.write_bytes(git('diff', '--binary', 'HEAD'))
print(f'BASE COMMIT {git("rev-parse", "HEAD").decode().strip()}')
