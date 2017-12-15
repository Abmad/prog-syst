CC = gcc


all : initial clients chefs mecanos 

initial : initial.c
	$(CC) -Wall initial.c -o initial

clients : clients.c
	$(CC) -Wall clients.c -o clients

chefs : chefs.c
	$(CC) -Wall chefs.c -o chefs

mecanos : mecanos.c
	$(CC) -Wall mecanos.c -o mecanos
clean :
	rm -f initial clients chefs mecanos
