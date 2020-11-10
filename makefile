run/server: src/server.c
	gcc src/server.c -o run/server
run/client: src/client.c
	gcc src/client.c -o run/client
build: run/server run/client
	echo 'build success'