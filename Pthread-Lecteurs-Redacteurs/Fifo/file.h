#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

typedef struct _cel {
  int* type;
  pthread_cond_t* cond;
  int* threadId;
  struct _cel* prev;
  struct _cel* next;
} Cel;

typedef struct _file {
  Cel* head;
  Cel* tail;
} File;

void init_cel(Cel* c, int type, int threadId);
void init_cel_lecteur(Cel* c, int threadId);
void init_cel_redacteur(Cel* c, int threadId);
void detruire_cel(Cel* c);
void print_cel(Cel* c);
void init_file(File* f);
void detruire_file(File* f);
pthread_cond_t* ajouter_lecteur_file(File* f, int threadId);
pthread_cond_t* ajouter_redacteur_file(File* f, int threadId);
void inserer_fin_file(File* f, Cel* c);
void supprimer_tete(File* f);
void supprimer_queue(File* f);
void supprimer_element(File* f, int threadId);
void afficher_file(File* f);
void afficher_file_reverse(File* f);
