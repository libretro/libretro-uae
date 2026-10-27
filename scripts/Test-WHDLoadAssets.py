"""Verify that both compiled helper formats contain the pinned WHDLoad release.

Run with a Python environment containing amitools. Normal core builds do not
need Python, amitools, a network download, or any user-installed WHDLoad files.
"""
import gzip
import hashlib
import io
import json
import re
import zipfile
from pathlib import Path
from amitools.fs.blkdev.BlkDevFactory import BlkDevFactory
from amitools.fs.ADFSVolume import ADFSVolume
from amitools.fs.FSString import FSString
ROOT = Path(__file__).resolve().parent.parent

def resource(name):
    text = (ROOT / 'retrodep' / name).read_text()
    data = bytes(int(s, 16) for s in re.findall(r'0x([0-9a-f]{2})', text))
    length = int(re.search(r'_len = (\d+);', text)[1])
    assert len(data) == length, name
    return data

def main():
    release = json.loads((ROOT / 'whdload/WHDLoad.release.json').read_text())
    base = ROOT / 'whdload/WHDLoad_files'
    with zipfile.ZipFile(io.BytesIO(resource('WHDLoad_files.zip.c'))) as archive:
        assert set(archive.namelist()) == {p.relative_to(base).as_posix() + ('/' if p.is_dir() else '') for p in base.rglob('*')}
        for p in base.rglob('*'):
            if p.is_file():
                assert archive.read(p.relative_to(base).as_posix()) == p.read_bytes(), p
    image = ROOT / 'whdload/WHDLoad.hdf'
    assert gzip.decompress(resource('WHDLoad_hdf.gz.c')) == image.read_bytes()
    bd = BlkDevFactory().open(str(image), read_only=True)
    volume = ADFSVolume(bd)
    try:
        volume.open()
        for path, info in release['assets'].items():
            data = (base / path).read_bytes()
            assert len(data) == info['bytes']
            assert hashlib.sha256(data).hexdigest() == info['sha256'], path
            assert volume.read_file(FSString(path)) == data, path
        main_binary = (base / 'C/WHDLoad').read_bytes()
        assert b'$VER: WHDLoad 20.1 [build 7190] (13.10.2026)' in main_binary
    finally:
        volume.close()
        bd.close()
    # Regeneration must not churn any embedded source or unrelated save image.
    import subprocess
    sources = [ROOT / 'retrodep/WHDLoad_files.zip.c', ROOT / 'retrodep/WHDLoad_hdf.gz.c']
    before = [p.read_bytes() for p in sources]
    subprocess.run(['python3', str(ROOT / 'scripts/Rebuild-WHDLoad.py')], check=True)
    assert before == [p.read_bytes() for p in sources]
    print('WHDLoad 20.1: source files, Files ZIP, HDF, release hashes, and deterministic regeneration passed.')

if __name__ == '__main__':
    main()
