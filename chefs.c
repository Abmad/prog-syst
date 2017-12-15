#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#define MSGSZ     128


/*
 * Declare the message structure.
 */

typedef struct Msg{
    long    mtype;
    char    mtext[MSGSZ];
}Msg;
//typedef struct message_bufmsg msgbuff;

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
    key_t cle;
    int qid;
    Msg msg;
    size_t msg_size;
    //Faire de facon infini

    while(1){
   
    mon_sigaction(SIGUSR1, signal_sigusr1);
    //Cle pour recuperer la file
    cle = ftok("toto",'a');
    qid = msgget(cle, IPC_CREAT| IPC_EXCL | 0660);
    
    if (cle < 0) {
        printf("Pb creation cle\n");
        exit(-1);
    }
    
    if (qid == -1)
    {
        printf("Pb creation file de message\n");
        exit(-1);
    }
    /*
     *Attente d'un client
     */
        
    if (msgrcv(cle, &msg, MSGSZ, 1, 0) < 0) {
        printf("Pb lecture de message\n");
        exit(-1);
    }
        msg_size = msgrcv(cle, &msg, MSGSZ, 1, 0);
    /*
     *Postage du travail sur la file de message (c'est a lui de determiner la duree et le nb outils)
     */
    if(msgsnd(cle,&msg,sizeof(msg.mtext),0)==-1){
        printf("Pb envoie de message\n");
        exit(-1);
    }else
        printf("message envoye depuis le chef au fil");
    /*
     *Attente de la fin du travail
     */
        printf("attente de terminaison du mecanico");
        wait(NULL);
    
    /*
     *Notification au client
     */
        
    }
    return 0;
}

