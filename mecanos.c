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
	pid_t pid, pid_caller;
	int semap, qid;
	int outil_1, outil_2, outil_3, outil_4;
	struct sembuf sem = {0,-1,SEM_UNDO};
	message msg;
	message msg_send;
	
	pid=getpid();
	cle = ftok("/tmp", 'S');
	if (cle==-1)
	{
		printf("(%d) Pb creation cle\n",pid);
		exit(-1);
    }

	mon_sigaction(SIGUSR1, signal_sigusr1);
	
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
	while(1)
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
        //fprintf(stderr,"\nMessage Bien recus !\n");
        pid_caller = msg.params.caller;
        //fprintf(stderr,"\n pid_caller: %d\n",pid_caller);

	
		outil_1 = msg.params.nbOutils_1;
        outil_2 = msg.params.nbOutils_2;
        outil_3 = msg.params.nbOutils_3;
        outil_4 = msg.params.nbOutils_4;
        
        //printf("Outil_1 : %d\n",outil_1);
        //printf("Outil_2 : %d\n",outil_2);
        //printf("Outil_3 : %d\n",outil_3);
        //printf("Outil_4 : %d\n",outil_4);
        
		/*
		*Reservation des outils (avec ensemble de semaphores)
		*/
		sem.sem_num = 0;
		sem.sem_op = -outil_1;
		semop(semap,&sem,1);
		sem.sem_num = 1;
		sem.sem_op = -outil_2;
		semop(semap,&sem,1);
		sem.sem_num = 2;
		sem.sem_op = -outil_3;
		semop(semap,&sem,1);
		sem.sem_num = 3;
		sem.sem_op = -outil_4;
		semop(semap,&sem,1);
	
		/*
		*Execution du travail (un sleep)
		*/
		sleep(msg.params.duree);
		
		/*
		*On rend les outils
		*/
		sem.sem_num = 0;
		sem.sem_op = outil_1;
		semop(semap,&sem,1);
		sem.sem_num = 1;
		sem.sem_op = outil_2;
		semop(semap,&sem,1);
		sem.sem_num = 2;
		sem.sem_op = outil_3;
		semop(semap,&sem,1);
		sem.sem_num = 3;
		sem.sem_op = outil_4;
		semop(semap,&sem,1);

		msg_send = msg;
		msg_send.msg_type = pid_caller;
	
		if(msgsnd(qid,&msg_send,MSGSZ,IPC_NOWAIT)==-1)
    	{
	  		fprintf(stderr,"Pb envoie de message\n");
            continue;
    	}
	 	//fprintf(stderr,"message envoye depuis le mecano au chef\n");	
	}
	
	return 0;
}
