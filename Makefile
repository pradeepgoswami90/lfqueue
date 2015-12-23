test:
	gcc -g test.c lfq.c -o test -lpthread
	
all:
	gcc -o lfq.o -c lfq.c -lpthread -g
	gcc -shared -o liblfq.so lfq.o -g

clean:
	rm -rf test liblfq.so *.o
