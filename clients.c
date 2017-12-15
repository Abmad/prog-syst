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
    msg_client msg_snd;
    message msg_rcv;
    mon_sigaction(SIGUSR1, signal_sigusr1);
    int cle_file;
    
    int cpt_test =0;
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
    
    for(;;)
    {
        
        /*
         *Transmission demande a ce chef
         */
        sprintf(msg_snd.msg, "demande num %d", cpt_test);
        msg_snd.msg_type = CLIENT_TO_CHEF;
        msg_snd.caller = getpid();
        if(msgsnd(cle_file,&msg_snd,MSGSZ,0)==-1)
        {
            printf("Pb envoie de message\n");
            exit(-1);
        }
        printf("message envoye depuis le client au chef\n");
        
        
        
        /*
         *Attente d'un client (Attente de la reponse du chef)
         */
        if (msgrcv(cle_file, &msg_rcv, MSGSZ, getpid(), 0) < 0)
        {
            printf("Pb lecture de message\n");
            exit(-1);
        }
        printf("reception de la demande traité du chef");
        
        /*
         *Afficher la demande et son resultat
         */
        
        fprintf(stderr,"cpt: %d",cpt_test);
    }
    
    
    
    /*
     *Mise en attente du chef le moins occupe
     */
    
    
    
    
    
    return 0;
}
