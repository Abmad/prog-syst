CC = gcc


all : init Clients Chefs Mecanos 

init : initial.c
	$(CC) -Wall initial.c -o init

Clients : clients.c
	$(CC) -Wall clients.c -o Clients

Chefs : chefs.c
	$(CC) -Wall chefs.c -o Chefs

Mecanos : mecanos.c
	$(CC) -Wall mecanos.c -o Mecanos
clean :
	rm -f init Clients Chefs Mecanos
