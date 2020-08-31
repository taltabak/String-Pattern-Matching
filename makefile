all:main.c
	gcc slist.c main.c pattern_matching.c -o ex1
all-GDB:main.c
	gcc -g slist.c -g main.c -g pattern_matching.c -o -wall ex1