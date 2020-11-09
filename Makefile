all:
	gcc sources/1/server.c -o sources/1/server -Wall -Werror
	gcc sources/1/client.c -o sources/1/client -Wall -Werror 
	gcc sources/2/server.c -o sources/2/server -Wall -Werror
	gcc sources/2/client.c -o sources/2/client -Wall -Werror 

clean:
	rm sources/1/server
	rm sources/1/client
	rm sources/2/server
	rm sources/2/client