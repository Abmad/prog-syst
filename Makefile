CC = gcc


all : pere fils

pere : pere.c
	$(CC) -Wall pere.c -o pere

fils : fils.c
	$(CC) -Wall fils.c -o fils

clean :
	rm -f pere fils cle.serv
