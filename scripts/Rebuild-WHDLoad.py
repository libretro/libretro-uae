"""Deterministically regenerate the WHDLoad ZIP/gzip C resources from checked-in assets."""
import gzip
import io
import zipfile
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent

def c_array(path, name, data):
    rows = [', '.join('0x%02x' % b for b in data[i:i+12]) for i in range(0, len(data), 12)]
    text = 'unsigned char %s[] = {\n' % name
    text += ',\n'.join('  ' + row for row in rows)
    text += '\n};\nunsigned int %s_len = %d;\n' % (name, len(data))
    path.write_text(text)

def main():
    buf = io.BytesIO()
    base = ROOT / 'whdload/WHDLoad_files'
    with zipfile.ZipFile(buf, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for p in sorted(base.rglob('*')):
            relative = p.relative_to(base).as_posix()
            info = zipfile.ZipInfo(relative + ('/' if p.is_dir() else ''), (2026, 10, 13, 0, 0, 0))
            info.create_system = 3
            info.external_attr = ((0o40755 if p.is_dir() else 0o100644) << 16) | (0x10 if p.is_dir() else 0)
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, b'' if p.is_dir() else p.read_bytes(), compresslevel=9)
    c_array(ROOT / 'retrodep/WHDLoad_files.zip.c', '___whdload_WHDLoad_files_zip', buf.getvalue())
    hdf = io.BytesIO()
    with gzip.GzipFile(filename='', mode='wb', fileobj=hdf, compresslevel=9, mtime=0) as packed:
        packed.write((ROOT / 'whdload/WHDLoad.hdf').read_bytes())
    c_array(ROOT / 'retrodep/WHDLoad_hdf.gz.c', '___whdload_WHDLoad_hdf_gz', hdf.getvalue())
    print('Regenerated bundled WHDLoad Files and HDF resources.')

if __name__ == '__main__':
    main()
