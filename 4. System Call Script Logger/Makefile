all: hscript

hscript: hscript.c
	gcc -Wall -o hscript hscript.c

clean:
	rm -f hscript

submission:
	gtar czvf prog4.tgz Makefile hscript.c README.md