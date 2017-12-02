makey: vdfdisk.c vdformat.c shell.c vdisk.o
	gcc -o vdfdisk vdfdisk.c vdisk.o
	gcc -o vdformat vdformat.c vdisk.o
	gcc -o shell shell.c vdisk.o

