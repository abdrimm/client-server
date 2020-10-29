all: 
  gcc sources/server.c -o sources/server
  gcc sources/client.c -o sources/client

clean:
	rm sources/server sources/client
