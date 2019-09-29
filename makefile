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

client:
	gcc -g -o client.out client.c -lpthread -lrt
	./client.out 88 99 66 55


server:
	gcc -g -o server.out server.c -lpthread -lrt
	./server.out 22 33 44 55
	
cli:
	gcc -g -o cli.out cli.c -lpthread -lrt -lm
	./cli.out


serv:
	gcc -g -o serv.out serv.c -lpthread -lrt -lm
	./serv.out