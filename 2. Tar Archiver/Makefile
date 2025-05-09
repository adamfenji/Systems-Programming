# !! Replace -O3 with -g for debugging !!
FLAGS = -Wall -O3

all: ctar utar

ctar:
	gcc $(FLAGS) ctar.c -o ctar
utar:
	gcc $(FLAGS) utar.c -o utar
clean:
	rm utar ctar
submission:
	gtar czvf prog2.tgz *
