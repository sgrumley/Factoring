build:
	cc corefunc.c -o core.out
	./core.out


test: 
	gcc -g -o facthread.out factors.c -lpthread -lm
	./facthread.out

debug:
	gcc -g -o facthreadbug.out factors.c -lpthread -lm
	gdb facthreadbug.out

tcore:
	gcc -g -o fre.out threadCore.c -lpthread -lm
	./fre.out

cli:
	gcc -g -o cli.out client.c
	./cli.out 88 99 66 55


serv:
	gcc -g -o serv.out server.c
	./serv.out 22 33 44 55
	
