#ifndef params_h
#define params_h

#define CLIENT_TO_CHEF 1
#define CHEF_TO_MECANO 2
#define MECANO_TO_CHEF 3
#define CHEF_TO_CLIENT 4
#define TAILLE 400
#define MSGSZ ((sizeof(int)*5)+sizeof(pid_t)+(sizeof(char)*TAILLE))

/*
 * Declaration des structures de message
 */

//structure du rest
typedef struct parametres{
    int duree;
    int nbOutils_1;
    int nbOutils_2;
    int nbOutils_3;
    int nbOutils_4;
    char msg[TAILLE];
    pid_t caller;
}parametres;
typedef struct message
{
    long msg_type;
    parametres params;
    
}message;



#endif /* params_h */
