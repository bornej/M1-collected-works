/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP5:
  Borne Jonathan

  Dans notre implémentation Fifo des lecteurs-rédacteurs
  on modélise la file des threads en attente par une liste chaînée de cellules.
  Chaque cellule correspond à un fil d'exécution endormi et contient:
  - L'id du thread
  - Le type de thread: {Lecteur = 0, Redacteur =1}
  - Une variable condition associée au thread pour pouvoir recevoir un signal de réveil.
*/
#include "file.h"

/*
  init_cel:
  - Spécification: prends en paramètre l'adresse d'une structure de données de type Cel,
  un entier type et un entier threadId.
  - Sémantique: Une Cellule dans la file correspond à un thread en attente.
  On Alloue la mémoire et initialise les attributs de la structure de Cellule avec
  les paramètres donnés, à savoir le type de thread {Lecteur = 0, Rédacteur = 1} et
  l'identifiant du thread. On associe à chaque thread de la file
  une variable condition pour recevoir un signal de réveil.
*/
void init_cel(Cel* c, int type, int threadId) {
  c->type = (int*) malloc(sizeof(int));
  c->cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  c->threadId = (int*) malloc(sizeof(int));
  c->prev = NULL;
  c->next = NULL;
  pthread_cond_init(c->cond,NULL);
  *c->type = type;
  *c->threadId = threadId;
}

/*
  init_file:
  - Spécification: prends en paramètre l'adresse d'une structure de données de type File.
  - Sémantique: Initialise la tête et la queue de la File à NULL.
*/
void init_file(File* f) {
  f->head = NULL;
  f->tail = NULL;
}

/* procédure intermédiaire */
void init_cel_lecteur(Cel* c, int threadId) {
  init_cel(c, 0, threadId);
}
/* procédure intermédiaire */
void init_cel_redacteur(Cel* c, int threadId) {
  init_cel(c, 1, threadId);
}

/*
  print_cel:
  - Spécification: Prends en paramètre l'adresse d'une structure de cellule.
  - Sémantique: Libère les espaces mémoires associés à la cellule.
*/
void detruire_cel(Cel* c) {
  free(c->type);
  pthread_cond_destroy(c->cond);
  free(c->cond);
  free(c->threadId);
  free(c);
  c->type = NULL;
  c->cond = NULL;
  c->threadId = NULL;
  c = NULL;
}

/*
  print_cel:
  - Spécification: Prends en paramètre l'adresse d'une structure de cellule.
  - Sémantique: Affiche les informations de la cellule.
*/
void print_cel(Cel* c) {
  printf("[ThreadId:%x, Type:%d] ",*c->threadId, *c->type);
}

/*
  print_cel_verbose:
  - Spécification: Prends en paramètre l'adresse d'une structure de cellule.
  - Sémantique: Version plus verbeuse que print_cel.
*/
void print_cel_verbose(Cel* c) {
  printf("[prev:%p |ThreadId:%x, Type:%d, cur:%p |next:%p] ",c->prev, *c->threadId, *c->type, c, c->next);
}

/*
  detruire_file:
  - Spécification: Prends en paramètre l'adresse d'une structure de file.
  - Sémantique: Parcours la file et libère l'espace mémoire de chacune des cellules.
  libère ensuite l'espace mémoire de la structure de file.
*/
void detruire_file(File* f) {
  if(f->head != NULL) {
    Cel* iterator = f->head;
    Cel* current;
    while (iterator != NULL) {
      current = iterator;
      iterator = iterator->next;
      detruire_cel(current);
    }
  }
  free(f);
}

/*
  ajouter_lecteur_file:
  - Spécification: Prends en paramètre l'adresse d'une structure de file et l'identifiant
  d'un thread.
  - Sémantique: Insère une cellule correspondant au thread d'identifiant threadid
  et de type lecteur à la fin de la file.
*/
pthread_cond_t* ajouter_lecteur_file(File* l, int threadId) {
  Cel* c = (Cel*) malloc(sizeof(Cel));
  init_cel_lecteur(c, threadId);
  inserer_fin_file(l, c);
  return c->cond;
}

/*
  ajouter_redacteur_file:
  - Spécification: Prends en paramètre l'adresse d'une structure de file et l'identifiant
  d'un thread.
  - Sémantique: Insère une cellule correspondant au thread d'identifiant threadid
  et de type rédacteur à la fin de la file.
*/
pthread_cond_t* ajouter_redacteur_file(File* l, int threadId) {
  Cel* c = malloc(sizeof(Cel));
  init_cel_redacteur(c, threadId);
  inserer_fin_file(l, c);
  return c->cond;
}

/*
  inserer_fin_file:
  - Spécification: Prends en paramètre l'adresse d'une structure de file et l'adresse
  d'une cellule.
  - Sémantique: Insère la cellule à la fin de la file.
*/
void inserer_fin_file(File* l, Cel* c) {
  if(l->tail == NULL) {
    l->head = c;
    l->tail = c;
    c->next = NULL;
    c->prev = NULL;
  } else {
    l->tail->next = c;
    c->next = NULL;
    c->prev = l->tail;
    l->tail = c;
  }
}

/*
  supprimer_tete:
  - Spécification: Prends en paramètre l'adresse d'une structure de file.
  - Sémantique: Supprime la cellule en tête de la file.
*/
void supprimer_tete(File* f) {
  if(f->head != NULL) {
    Cel* c = f->head;
    if( f->head->next == NULL) {
      f->head = f->tail = NULL;
    } else if (f->head->next == f->tail) {
      f->head = f->tail;
      f->tail = f->head;
      f->head->prev = NULL;
    } else {
      f->head = f->head->next;
      f->head->prev = NULL;
    }
    detruire_cel(c);
  }
}

/*
  supprimer_queue:
  - Spécification: Prends en paramètre l'adresse d'une structure de file.
  - Sémantique: Supprime la cellule en queue de la file.
*/
void supprimer_queue(File* f){
  if(f->tail != NULL) {
    Cel* c = f->tail;
    if(f->tail == f->head) {
      f->head = NULL;
      f->tail = NULL;
    } else {
      f->tail = f->tail->prev;
      f->tail->next = NULL;
    }
    detruire_cel(c);
  }
}

/*
  supprimer_element:
  - Spécification: Prends en paramètre l'adresse d'une structure de file et
    l'identifiant d'un thread.
  - Sémantique: Suppression de l'élément de la file f portant l'identifiant threadId.
    Un identifiant ne peut être présent qu'une seule fois dans la file.
  - Implémentation: On parcours la file f à l'aide d'un itérateur
    sur les adresses des cellules. Si l'identifiant est présent dans la file,
    à l'issue du parcours, l'itérateur pointe vers la cellule à supprimer.
    Sinon l'itérateur vaut NULL. On distingue trois cas selon que l'itérateur
    est en tête, en queue ou à une position quelconque dans la file.
*/
void supprimer_element(File* f, int threadId) {
  Cel* iterator = f->head;
  while(iterator !=NULL && *iterator->threadId != threadId) {
    iterator = iterator->next;
  }
    if(iterator != NULL) {
    if (iterator == f->tail) {
      supprimer_queue(f);
    } else if(iterator == f->head){
      supprimer_tete(f);
    } else {
      Cel* prev = iterator->prev;
      (iterator->prev)->next = iterator->next;
      (iterator->next)->prev = prev;
      detruire_cel(iterator);
    }
  }
}

/*
  afficher_file:
  - Spécification: Prends en paramètre l'adresse d'une structure de file.
  - Sémantique: Affiche les informations des cellules de la file.
*/
void afficher_file(File* f) {
  Cel* iterator = f->head;
  if(iterator == NULL) {
    printf("     la file est vide.\n");
  } else {
    printf("     File d'attente:    \n");
    printf("     ");
    while (iterator !=NULL) {
      print_cel(iterator);
      iterator = iterator->next;
    }
    printf("\n");
  }
}

/*
  afficher_file_verbose:
  - Spécification: Prends en paramètre l'adresse d'une structure de file.
  - Sémantique: Version plus verbeuse que afficher_file.
*/
void afficher_file_verbose(File* f) {
  Cel* iterator = f->head;
  if(iterator == NULL) printf("     la file est vide.\n");
  printf("     File     \n");
  printf("     Head:%p, Tail:%p\n", f->head, f->tail);
  printf("     ");
  while (iterator !=NULL) {
    print_cel(iterator);
    iterator = iterator->next;
  }
  printf("\n");
}

/* Procedure test */
void afficher_file_reverse(File* f) {
  Cel* iterator = f->tail;
  int i = 0;
  while (iterator !=NULL) {
    print_cel(iterator);
    i++;
    iterator = iterator->prev;
  }
  printf("\n");
}
