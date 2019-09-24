build:
	cc corefunc.c -o core
	./core


test: 
	gcc -g -o facthread factors.c -lpthread -lm
	./facthread

debug:
	gcc -g -o facthreadbug factors.c -lpthread -lm
	gdb facthreadbug

tcore:
	gcc -g -o fre threadCore.c -lpthread -lm
	./fre

