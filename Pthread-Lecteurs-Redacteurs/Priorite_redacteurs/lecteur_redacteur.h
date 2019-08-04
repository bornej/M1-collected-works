/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP5:
  Borne Jonathan
*/
#include <pthread.h>

typedef struct _lecteur_redacteur_t {
  pthread_mutex_t* mutex;
  pthread_cond_t* fileL;
  pthread_cond_t* fileR;
  int* nbRs;
  int* nbLa;
  int* enEcriture;

} lecteur_redacteur_t;

void initialiser_lecteur_redacteur(lecteur_redacteur_t* lr);
void detruire_lecteur_redacteur(lecteur_redacteur_t* lr);
void debut_redaction(lecteur_redacteur_t* lr);
void fin_redaction(lecteur_redacteur_t* lr);
void debut_lecture(lecteur_redacteur_t* lr);
void fin_lecture(lecteur_redacteur_t* lr);
