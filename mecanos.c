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
#include "params.h"
#include <signal.h>


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

// 1 param, numero d'ordre 
int main(int argc, char * argv[], char * envp[])
{

	key_t cle;
	pid_t pid;
	int semap, qid;
	//int outil_1, outil_2, outil_3, outil_4;
	message msg;
	message msg_send;
	pid=getpid();
    pid_t pid_caller;
	cle = ftok("/tmp", 'S');
	if (cle==-1)
	{
		printf("(%d) Pb creation cle\n",pid);
		exit(-1);
    	}


	

	/*
	*Recuperation semaphores
	*/
    semap = semget(cle,1,0);
    if (semap==-1)
    {
		printf("(%d) Pb recuperation semaphore\n",pid);
		exit(-1);
    }
	
	
	/*
	*Recuperation file de message
	*/
	qid= msgget(cle, 0666);
    if (qid == -1)
    {
    	printf("Erreur recuperation de file\n");
	exit(-1);
    }
	
	
	//Faire de facon infinie
	for(;;)
	{
	
	/*
		*Reception d'un travail dans la file de message, un travail est (duree +nb outils de chaque type)
		*/
	
		
        //Le CHEF_TO_MECANO ici correspond au type de messages à receptionner
        if (msgrcv(qid, &msg, MSGSZ, CHEF_TO_MECANO, 0) == -1)
        {
           fprintf(stderr,"Erreur de lecture dans la file\n");
            continue;
        }
        fprintf(stderr,"\nMessage Bien recus !\n");
        pid_caller = msg.params.caller;
        fprintf(stderr,"\n pid_caller: %d\n",pid_caller);

	
		/*
		*Reservation des outils (avec ensemble de semaphores)
		*/
	
		/*
		*Execution du travail (un sleep)
		*/
		sleep(msg.params.duree);
	
		/*
		*Notification de la fin du travail au chef a l'origine du travail
		*/

	msg_send = msg;
	msg_send.msg_type = pid_caller;
	
	if(msgsnd(qid,&msg_send,MSGSZ,IPC_NOWAIT)==-1)
    	{
	  fprintf(stderr,"Pb envoie de message\n");
            continue;
    	}
	 fprintf(stderr,"message envoye depuis le mecano au chef\n");	
	}
	
	return 0;
}
