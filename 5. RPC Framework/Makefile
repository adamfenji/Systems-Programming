all: rserver rclient rclient1 rclient2

rserver: rserver.c
	gcc -g -Wall -o rserver rserver.c

rclient: rclient.c
	gcc -g -Wall -c rclient.c

rclient1: rclient rclient1.c
	gcc -g -Wall -o rclient1 rclient1.c rclient.o

rclient2: rclient rclient2.c
	gcc -g -Wall -o rclient2 rclient2.c rclient.o

clean:
	rm rclient.o rserver rclient1 rclient2

submission:
	gtar czvf prog5.tgz Makefile rserver.c rclient.c rclient1.c rclient2.c README.md