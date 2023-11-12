all:
	gcc -Wall -g -c common.c
	gcc -Wall -g client.c common.o -o client
	gcc -Wall -g server-mt.c common.o -lpthread -o server-mt

clean:
	rm common.o client server server-mt
