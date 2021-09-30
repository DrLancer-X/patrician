/c/gbdk/bin/lcc -c -o test.o test.c
/c/gbdk/bin/lcc -c -o SoundSystem.o SoundSystem.c
/c/gbdk/bin/lcc -c -o Audio.o Audio.c
/c/gbdk/bin/lcc -c -o Battle.o Battle.c
/c/gbdk/bin/lcc -c -o MusicInformation.o MusicInformation.c
/c/gbdk/bin/lcc -c -o MusicData2.o MusicData2.c
/c/gbdk/bin/lcc -c -o MusicData2.o MusicData2.c
/c/gbdk/bin/lcc -c -o MusicData3.o MusicData3.c
for F in GenData*.c
do
  /c/gbdk/bin/lcc -c -o `basename $F .c`.o $F
done
/c/gbdk/bin/lcc -c -o Globals.o Globals.c
/c/gbdk/bin/lcc -c -o Exploration.o Exploration.c
/c/gbdk/bin/lcc -Wm-yC -Wm-yt27 -Wm-yo128 -Wm-ya16 -o test.gbc test.o SoundSystem.o MusicInformation.o MusicData2.o MusicData3.o GenData*.o Globals.o Exploration.o Audio.o Battle.o
