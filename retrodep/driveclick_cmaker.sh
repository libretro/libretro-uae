echo "" > driveclick_wav.c
cd driveclick_wav/
xxd -i drive_click.wav >> ../driveclick_wav.c
xxd -i drive_snatch.wav >> ../driveclick_wav.c
xxd -i drive_spin.wav >> ../driveclick_wav.c
xxd -i drive_spinnd.wav >> ../driveclick_wav.c
xxd -i drive_startup.wav >> ../driveclick_wav.c
cd ..
