cd ../whdload/WHDLoad_files/
zip -9 -r -X ../WHDLoad_files.zip .
cd ../../retrodep/
xxd -i ../whdload/WHDLoad_files.zip WHDLoad_files.zip.c
rm ../whdload/WHDLoad_files.zip

gzip -9 -f -k -N ../whdload/WHDLoad.prefs
xxd -i ../whdload/WHDLoad.prefs.gz WHDLoad_prefs.gz.c
rm ../whdload/WHDLoad.prefs.gz
