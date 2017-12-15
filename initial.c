#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define TAILLE_MAX 100

int semap, file_init;

/*
*Attente Ctrl-C pour nettoyage et fin
*/
void signal_sigint(int s)
{
	printf("FIN\n");
	
	//Netoyages
	semctl(semap,1,IPC_RMID,NULL);
	msgctl (file_init, IPC_RMID, NULL);
	kill(0, SIGUSR1);
	
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

// 6 param (nb chefs, nb mecanos, nb outils de chacun des 4 types)  
int main(int argc, char * argv[], char * envp[])
{
	int nbChefs, nbMecanos, nb_1, nb_2, nb_3, nb_4, itFor;
	int nbOutils[4] = {1};
	pid_t pid;
	pid_t pidChefs[TAILLE_MAX];
	pid_t pidMecanos[TAILLE_MAX];
	pid_t pidClients[TAILLE_MAX];
	key_t cle;
	int res_init;
	mon_sigaction(SIGINT, signal_sigint);
	
	/*
	*Verifications arguments
	*/
	if(argc != 7) 
	{
		printf("Erreur, nombre d'arguments invalide, les arguments doivent etre :\n[nb_Chefs] [nb_Mecanos] [nb_Outils_1] [nb_Outils_2] [nb_Outils_3] [nb_Outils_4]\n");
		return -1;
	}
	nbChefs = atoi(argv[1]);
	nbMecanos = atoi(argv[2]);
	nb_1 = atoi(argv[3]);
	nb_2 = atoi(argv[4]);
	nb_3 = atoi(argv[5]);
	nb_4 = atoi(argv[6]);
	
	/*
	*Creation IPC (File de message + ensemble de semaphores)
	*/
	//Cle
	cle = ftok("toto",'a');
	
	//Ensemble de semaphores
	semap = semget(cle,1,IPC_CREAT | IPC_EXCL | 0660);
    if (semap==-1)
    {
		printf("Pb creation semaphore ou il existe deja\n");
		exit(-1);
    }

    /* On l'initialise :                                     */
    res_init = semctl(semap,1,SETALL,nbOutils);
    if (res_init==-1)
    {
		printf("Pb initialisation semaphore\n");
		/* On detruit les IPC deje crees : */
		semctl(semap,1,IPC_RMID,NULL);
		exit(-1);
    }
	
	//File de messages
    file_init = msgget(cle, IPC_CREAT| IPC_EXCL | 0660);
    if (file_init == -1)
    {
    	printf("Erreur creation de file\n");
    	semctl(semap,1,IPC_RMID,NULL);
    	msgctl (file_init, IPC_RMID, NULL);
	}
	
	
	/*
	*Creation chefs d'atelier
	*/
    fprintf(stderr,"Lancement des Chefs");
    for(itFor=0; itFor<nbChefs; itFor++)
    {
		fprintf(stderr,".");
		
		char parameter[32];
		sprintf(parameter, "%d", itFor);
		char * param[] = {"Chefs", parameter, argv[4], argv[5], argv[6], argv[7], NULL};

		pid = fork();
		pidChefs[itFor] = pid;  
		if (pid==-1) break;  /* Probleme a la creation du i-ieme fils, on arrete les fork */
		if (pid==0)
		{
		    /* c'est le i-eme fils   */
		    execve("Chefs", param, envp);
		    /* en principe jamais atteint */
		    exit(-1);
		}
    }
    fprintf(stderr,"fait\n");
    
	
	/*
	*Creation mecanos
	*/
	fprintf(stderr,"Lancement des Mecaniciens");
    for(itFor=0; itFor<nbMecanos; itFor++)
    {
		fprintf(stderr,".");
		
		char parameter[32];
		sprintf(parameter, "%d", itFor);
		char * param[] = {"Mecanos", parameter, NULL};
		
		pid = fork(); 
		pidMecanos[itFor] = pid;  
		if (pid==-1) break;  /* Probleme a la creation du i-ieme fils, on arrete les fork */
		if (pid==0)
		{
		    /* c'est le i-eme fils   */
		     execve("Mecanos", param, envp);
		    /* en principe jamais atteint */
		    exit(-1);
		}
    }
    fprintf(stderr,"fait\n");
	
	/*
	*Creation clients de facon infinie
	*/
	fprintf(stderr,"Lancement des Clients");
	char * param[] = {"Clients", argv[1], NULL};
    while(1)
    {
		fprintf(stderr,".");
	
		pid = fork();  
		if (pid==-1) break;  /* Probleme a la creation du i-ieme fils, on arrete les fork */
		if (pid==0)
		{
		    /* c'est le i-eme fils   */
		     execve("Clients", param, envp);
		    /* en principe jamais atteint */
		    exit(-1);
		}
		sleep(2);
    }
}
