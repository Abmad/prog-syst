#ifndef params_h
#define params_h

#define CLIENT_TO_CHEF 1
#define CHEF_TO_MECANO 2
#define MECANO_TO_CHEF 3
#define CHEF_TO_CLIENT 4
#define MSGSZ ((sizeof(int)*5)+sizeof(pid_t))
#define TAILLE 400
/*
 * Declaration des structures de message
 */

//structure client_to_chef
typedef struct msg_client{
    long msg_type;
    char msg[TAILLE];
    pid_t caller;
}msg_client;
//structure du rest
typedef struct parametres{
    int duree;
    int nbOutils_1;
    int nbOutils_2;
    int nbOutils_3;
    int nbOutils_4;
    pid_t caller;
}parametres;
typedef struct message
{
    long msg_type;
    parametres params;
    
}message;



#endif /* params_h */
