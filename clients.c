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

// 1 param, le nb de chef  
int main(int argc, char * argv[], char * envp[])
{	
	mon_sigaction(SIGUSR1, signal_sigusr1);

	while(1)
	{
	
	}
	
	/*
	*Recuperation IPC
	*/
	
	/*
	*Mise en attente du chef le moin occupe
	*/
	
	/*
	*Transmission demande a ce chef
	*/
	
	/*
	*Attente de la reponse du chef
	*/
	
	/*
	*Afficher la demande et son resultat
	*/

	return 0;
}
