test:
	gcc -g test.c lfq.c -o test -lpthread
	
all:
	gcc -o lfq.o -c lfq.c -lpthread -fPIC
	gcc -shared -o liblfq.so lfq.o -fPIC

clean:
	rm -rf test liblfq.so *.o
