#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
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

// 1 param, le nb de chef  
int main(int argc, char * argv[], char * envp[])
{	
    key_t cle;
    message msg_snd;
    message msg_rcv;
    mon_sigaction(SIGUSR1, signal_sigusr1);
    int cle_file;
    pid_t cl_pid;
    cl_pid = getpid();
    /*
     *Recuperation IPC
     */
    // recuperation de la file
    cle = ftok("/tmp", 'S');
    if (cle == -1)
    {
        printf("Pb creation cle\n");
        exit(-1);
    }
    
    cle_file=msgget(cle, 0666);
    if ( cle_file== -1)
    {
        printf("Pb creation file de message\n");
        exit(-1);
    }
    
    
    
    /*
     *Transmission demande a ce chef
     */
    sprintf(msg_snd.params.msg, "demande num %d",cl_pid);
    msg_snd.msg_type = CLIENT_TO_CHEF;
    msg_snd.params.caller = cl_pid;
    if(msgsnd(cle_file,&msg_snd,MSGSZ,IPC_NOWAIT)==-1)
    {
        printf("Pb envoie de message\n");
        exit(-1);
    }
    fprintf(stderr,"message envoye depuis le client au chef\n");
    
    
    
    /*
     *Attente d'un client (Attente de la reponse du chef)
     */
    if (msgrcv(cle_file, &msg_rcv, MSGSZ, cl_pid, IPC_NOWAIT) < 0)
    {
        printf("Pb lecture de message\n");
        exit(-1);
    }
    fprintf(stderr,"reception de la demande traité du chef");
    
    /*
     *Afficher la demande et son resultat
     */
    fprintf(stderr,"\n\n Demande :%s  ",msg_rcv.params.msg);
    fprintf(stderr,"Duree :%d  ",msg_rcv.params.duree);
    fprintf(stderr,"NbOutils_1:%d  ",msg_rcv.params.nbOutils_1);
    fprintf(stderr,"NbOutils_2:%d  ",msg_rcv.params.nbOutils_2);
    fprintf(stderr,"NbOutils_3:%d  ",msg_rcv.params.nbOutils_3);
    fprintf(stderr,"NbOutils_4:%d  \n",msg_rcv.params.nbOutils_4);
    
    
    
    
    /*
     *Mise en attente du chef le moins occupe
     */
    
    
    
    
    
    return 0;
}
