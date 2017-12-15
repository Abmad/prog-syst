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


//Structure du message
struct message
{
    long msg_type;
    int duree;
    int nbOutils_1;
    int nbOutils_2;
    int nbOutils_3;
    int nbOutils_4;
};

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
	int semap, file_init, read;
	int outil_1, outil_2, outil_3, outil_4;
	
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
	file_init= msgget(cle, 0666);
    if (file_init == -1)
    {
    	printf("Erreur recuperation de file\n");
	}
	
	//Faire de facon infinie
	while(1)
	{
		/*
		*Reception d'un travail dans la file de message, un travail est (duree +nb outils de chaque type)
		*/
		struct message msg;
        read= msgrcv(file_init, &msg, 5*sizeof(int), 1, 0);//Le 1 ici correspond au type de messages à receptionner
        if (read == -1)
        {
            printf("Erreur de lecture dans la file\n");
        }
	
		/*
		*Reservation des outils (avec ensemble de semaphores)
		*/
	
		/*
		*Execution du travail (un sleep)
		*/
		sleep(msg.duree);
	
		/*
		*Notification de la fin du travail au chef a l'origine du travail
		*/
	}
	
	return 0;
}
