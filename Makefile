all:server.c client.c
	gcc -Wall -o ssl-server server.c -L/usr/lib -lssl -lcrypto
	gcc -Wall -o ssl-client client.c -L/usr/lib -lssl -lcrypto
clean:ssl-server ssl-client
	rm ssl-server ssl-client
