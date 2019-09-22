build:
	cc corefunc.c -o core
	./core.exe


test: 
	gcc -g -o facthread factors.c -lpthread -lm
	./facthread.exe

debug:
	gcc -g -o facthreadbug factors.c -lpthread -lm
	gdb facthreadbug.exe

tbug:
	gcc -g -o fre OGfactors.c -lpthread -lm
	./fre.exe 100 4

