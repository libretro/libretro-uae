gzip -9 -f -k ../whdload/WHDLoad.hdf 
xxd -i ../whdload/WHDLoad.hdf.gz WHDLoad_hdf.gz.c
rm ../whdload/WHDLoad.hdf.gz

gzip -9 -f -k ../whdload/WHDSaves.hdf 
xxd -i ../whdload/WHDSaves.hdf.gz WHDSaves_hdf.gz.c
rm ../whdload/WHDSaves.hdf.gz

gzip -9 -f -k ../whdload/WHDLoad.prefs
xxd -i ../whdload/WHDLoad.prefs.gz WHDLoad_prefs.gz.c
rm ../whdload/WHDLoad.prefs.gz
