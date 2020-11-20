all:
	gcc sources/1/server.c -o sources/1/server -Wall -Werror
	gcc sources/1/client.c -o sources/1/client -Wall -Werror 
	gcc sources/2/server.c -o sources/2/server -Wall -Werror
	gcc sources/2/client.c -o sources/2/client -Wall -Werror 
	gcc sources/3/server.c -o sources/3/server -Wall -Werror
	gcc sources/3/client.c -o sources/3/client -Wall -Werror 
	gcc sources/4/server.c -o sources/4/server -Wall -Werror
	gcc sources/4/client.c -o sources/4/client -Wall -Werror 

clean:
	rm sources/1/server
	rm sources/1/client
	rm sources/2/server
	rm sources/2/client
	rm sources/3/server
	rm sources/3/client
	rm sources/4/server
	rm sources/4/client
