cd ../whdload/WHDLoad_files/
zip -9 -r -X ../WHDLoad_files.zip .
cd ../../retrodep/
xxd -i ../whdload/WHDLoad_files.zip WHDLoad_files.zip.c
rm ../whdload/WHDLoad_files.zip

gzip -9 -f -k -N ../whdload/WHDLoad.hdf
xxd -i ../whdload/WHDLoad.hdf.gz WHDLoad_hdf.gz.c
rm ../whdload/WHDLoad.hdf.gz

gzip -9 -f -k -N ../whdload/WHDSaves.hdf
xxd -i ../whdload/WHDSaves.hdf.gz WHDSaves_hdf.gz.c
rm ../whdload/WHDSaves.hdf.gz

gzip -9 -f -k -N ../whdload/WHDLoad.prefs
xxd -i ../whdload/WHDLoad.prefs.gz WHDLoad_prefs.gz.c
rm ../whdload/WHDLoad.prefs.gz
