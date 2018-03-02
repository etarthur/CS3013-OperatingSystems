all: mc0 mc1 mc2

mc0: mc0.o
	gcc -o mc0 mc0.o

mc0.o: mc0.c
	gcc -Wall -c mc0.c

mc1: mc1.o
	gcc -o mc1 mc1.o

mc1.o: mc1.c
	gcc -Wall -c mc1.c

mc2: mc2.o
	gcc -o mc2 mc2.o

mc2.o: mc2.c
	gcc -Wall -c mc2.c

clean:
	touch mc0
	touch mc1
	touch mc2
	touch mc2.o
	touch mc1.o
	touch mc0.o
	rm mc2.o mc1.o mc0.o mc0 mc1 mc2
