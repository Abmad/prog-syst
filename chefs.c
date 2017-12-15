/**
 *0 pour clients
 *1 pour chefs
 *2 pour mecanos
 **/
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include "params.h"
#include <time.h>
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


// 5 param (num d'ordre, nb outils de chaque type dispo)
int main(int argc, char * argv[], char * envp[])
{
    /*
     *Recuperation IPC
     */
    pid_t this_pid = getpid();
    srand(this_pid);
    //long type = getpid();
    
    int r = rand();
    key_t cle;
    int qid;
    mon_sigaction(SIGUSR1, signal_sigusr1);
    message msg_snd,msg_rcv;
    message msg_client_rcv;
    int max_nbOutils_1,max_nbOutils_2,max_nbOutils_3,max_nbOutils_4;
    max_nbOutils_1 = atoi(argv[2]);
    max_nbOutils_2 = atoi(argv[3]);
    max_nbOutils_3 = atoi(argv[4]);
    max_nbOutils_4 = atoi(argv[5]);
    msg_snd.params.nbOutils_1 = r%max_nbOutils_1;
    msg_snd.params.nbOutils_2 = r%max_nbOutils_2;
    msg_snd.params.nbOutils_3 = r%max_nbOutils_3;
    msg_snd.params.nbOutils_4 = r%max_nbOutils_4;
    msg_snd.msg_type = CHEF_TO_MECANO;
    msg_snd.params.duree = r%5;//(r*1500)+500;
    msg_snd.params.caller = getpid();
    pid_t client_pid;
    //Faire de facon infini
    //Cle pour recuperer la file
    cle = ftok("/tmp", 'S');
    if (cle == -1)
    {
        printf("Pb creation cle\n\n");
        exit(-1);
    }
    
    qid = msgget(cle, 0666);
    if (qid == -1)
    {
        printf("Pb creation file de message\n\n");
        exit(-1);
    }
    
    for(;;)
    {
        /*
         *Attente d'un client
         */
        
        if (msgrcv(qid, &msg_client_rcv, MSGSZ, CLIENT_TO_CHEF, 0) < 0)
        {
            printf("Pb lecture de message\n\n");
            exit(-1);
        }
        printf("reception de la demande du client qui est la suivante: %s\n\n",msg_client_rcv.params.msg);
        client_pid = msg_client_rcv.params.caller;
        /*
         *Postage du travail sur la file de message (c'est a lui de determiner la duree et le nb outils)
         */
        
        if(msgsnd(qid,&msg_snd,MSGSZ,0)==-1)
        {
            fprintf(stderr,"Pb envoie de message\n\n");
            exit(-1);
        }
        fprintf(stderr,"message envoye depuis le chef au mecano\n\n");
        fprintf(stderr,"\nthis_pid: %d\n",this_pid);
        
        /*
         *Attente de la fin du travail
         */
        fprintf(stderr,"attente de terminaison du mecanico\n\n");
        //sleep(10);
        
        //attente de la reponse du mecanico
        
        if (msgrcv(qid, &msg_rcv, MSGSZ, this_pid, 0)< 0)
        {
            fprintf(stderr,"Pb lecture de message\n");
            exit(-1);
        }
        
        fprintf(stderr,"Message recu depuis le mecano\n");
        
        msg_snd=msg_rcv;
        msg_snd.msg_type = client_pid;
        
        if(msgsnd(qid,&msg_snd,MSGSZ,0)==-1)
        {
            fprintf(stderr,"Pb envoie de message\n\n");
            exit(-1);
        }
        fprintf(stderr,"message envoye depuis le chef au Client\n\n");
        /*
         *Notification au client
         */
        
    }
    return 0;
}


