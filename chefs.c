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
#define MSGSZ sizeof(int)*5


/*
 * Declare the message structure.
 */
typedef struct msg_client_recu{
  long msg_type;
  char * msg;
}msg_client_recu;
typedef struct Msg
{
  long msg_type;
  int duree;
  int nbOutils_1;
  int nbOutils_2;
  int nbOutils_3;
  int nbOutils_4;
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
  srand(getpid());   // should only be called once
  int r = rand();  
  key_t cle;
  int qid;
  Msg msg_snd,msg_rcv;
  int max_nbOutils_1,max_nbOutils_2,max_nbOutils_3,max_nbOutils_4;
  msg_client_recu msg_client_rcv;
  max_nbOutils_1=atoi(argv[2]);
  max_nbOutils_2=atoi(argv[3]);
  max_nbOutils_3=atoi(argv[4]);
  max_nbOutils_4=atoi(argv[5]);
  
  msg_snd.nbOutils_1 = (r*(max_nbOutils_1-1))+1;
  msg_snd.nbOutils_2 = (r*(max_nbOutils_2-1))+1;
  msg_snd.nbOutils_3 = (r*(max_nbOutils_3-1))+1;
  msg_snd.nbOutils_4 = (r*(max_nbOutils_4-1))+1;
  msg_snd.msg_type = CHEF_TO_MECANO;
  mon_sigaction(SIGUSR1, signal_sigusr1);
  msg_snd.duree = (r*1500)+500;
  //Faire de facon infini
  while(1)
    {
      
      
      //Cle pour recuperer la file
      cle = ftok("/tmp", 'S');
      if (cle == -1) 
    	{
	  printf("Pb creation cle\n");
	  exit(-1);
	}
      
      qid = msgget(cle, 0666);
      if (qid == -1)
    	{
	  printf("Pb creation file de message\n");
	  exit(-1);
    	}
      
      /*
       *Attente d'un client
       */   
      if (msgrcv(cle, &msg_client_rcv, MSGSZ, CLIENT_TO_CHEF, 0) < 0) 
    	{
	  printf("Pb lecture de message\n");
	  exit(-1);
        }
      printf("reception de la demande du client qui est la suivante: %s",msg_client_rcv.msg);
      
    	/*
	 *Postage du travail sur la file de message (c'est a lui de determiner la duree et le nb outils)
	 */
      
      if(msgsnd(cle,&msg_snd,MSGSZ,IPC_NOWAIT)==-1)
    	{
	  printf("Pb envoie de message\n");
	  exit(-1);
    	}else printf("message envoye depuis le chef au mecano\n");
      
      
      /*
       *Attente de la fin du travail
       */
      printf("attente de terminaison du mecanico");
      // wait(NULL);
      //attente de la reponse du mecanico
      if (msgrcv(cle, &msg_rcv, MSGSZ, MECANO_TO_CHEF, 0) < 0) 
    	{
	  printf("Pb lecture de message\n");
	  exit(-1);
        }

      printf("Message recu depuis le mecano");

      msg_snd=msg_rcv;
      msg_snd.msg_type = CHEF_TO_CLIENT;
      if(msgsnd(cle,&msg_snd,MSGSZ,IPC_NOWAIT)==-1)
	{
	  printf("Pb envoie de message\n");
	  exit(-1);
    	}else printf("message envoye depuis le chef au Client\n");
      /*
       *Notification au client
       */
      
    }
  return 0;
}


