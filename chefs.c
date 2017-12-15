#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

/*
*Attente SIGUSR1 pour nettoyage et fin
*/
void signal_sigusr1(int s)
{
	printf("FIN\n");
	
	//Netoyages
	
	exit(0);
}

void mon_sigaction(int signal, void (*f)(int))
{
	struct sigaction action;

	action.sa_handler = f;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(signal, &action, NULL);
}


// 5 param (num d'ordre, nb outils de chaque type dispo)
int main(int argc, char * argv[], char * envp[])
{
	mon_sigaction(SIGUSR1, signal_sigusr1);

	while(1)
	{
	
	}

	/*
	*Recuperation IPC
	*/
	
	
	//Faire de facon infini
	/*
	*Attente d'un client
	*/
	
	/*
	*Postage du travail sur la file de message (c'est a lui de determiner la duree et le nb outils)
	*/
	
	/*
	*Attente de la fin du travail
	*/
	
	/*
	*Notification au client
	*/

	return 0;
}
