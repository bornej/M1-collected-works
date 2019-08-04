/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP5:
  Borne Jonathan

  Solution au problème des lecteurs-rédacteurs version FIFO.
  - Énoncé général:
    Dans ce problème, des lecteurs et des rédacteurs souhaitent accéder de manière concurrente à une donnée.
    On n'autorise qu'un seul rédacteur à écrire la donnée à la fois.
    On autorise plusieurs lecteurs à lire la donnée en même temps.
    Lecteurs et rédacteurs ne peuvent pas accéder à la donnée simultanément.

    Chaque lecteur et rédacteur est modélisé par un fil d'exécution.
    Un fil peut être dans l'un des deux états {Actif, Sommeil}.
    Si un lecteur tente d'accéder à la ressource et qu'un rédacteur est actif on l'endort.
    Si un rédacteur tente d'accéder à la ressource pendant que des lecteurs ou un rédacteur sont actifs.
    On définit une file d'attente file dans laquelle on place les Lecteurs et rédacteurs en sommeil.
    Les fils d'exécutions endormis seront ensuite réveillés selon leur ordre d'arrivée lorsque la ressource se libérera.

  - Implémentation FIFO:
    On maintient une file mémorisant les lecteurs et rédacteurs en attente.
    La file permet de mémoriser l'ordre dans lequel les fils d'exécutions ont été endormis.
    On fait des insertions en queue de liste, ainsi la tête de liste pointe vers
    le premier fil à avoir été endormi.

  - Exemple Fifo:
    On note R et L, rédacteurs et lecteurs respectivement.
    Supposons que l'on ait la file d'attente f = [R1, L1, L2, L3, L4, R2, L5]
    et qu'un rédacteur R0 soit actif.
    (a) Lorsque R0 finit son écriture il va réveiller le premier fil en attente.
      R1 devient actif et sort de la file d'attente.
      -> f = [L1, L2, L3, L4, R2, L5].
    (b) Lorsqu'il finit sa rédaction, R1 réveille L1, L2, L3, L4 afin qu'ils puissent
      s'exécuter en parallèle.
      Les lecteurs L1, L2, L3, L4 deviennent actifs et sortent de la file d'attente.
      -> f = [R2, L5].
    (c) Le dernier lecteur actif réveillera le rédacteur R2 et ainsi de suite.

  - Sémantique des variables:
    - L'entier nbLs compte le nombre de lecteurs en sommeil.
    - L'entier nbLa compte le nombre de lecteurs actifs.
    - L'entier enEcriture vaut 1 si un rédacteur est actif 0 sinon.
    - mutex est un verrou que l'on peut placer pour garantir un accès exclusif à la structure lecteur_redacteur_t.
    - f est une file d'attente.

  - File d'attente (voir file.h):
    On modélise la file d'attente par une liste chainée de cellules.
    Chaque cellule correspond à un fil d'execution endormi et contient:
    - L'id du thread
    - Le type de thread: {Lecteur = 0, Redacteur =1}
    - Une variable condition associée au thread pour pouvoir recevoir un signal de réveil.
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "lecteur_redacteur.h"

/*
  initialiser_lecteur_redacteur:
  - Spécification: prends en paramètre l'adresse d'une structure de données de type lecteur_redacteur_t
  - Sémantique: Alloue la mémoire et initialise les attributs de la structure.
*/
void initialiser_lecteur_redacteur(lecteur_redacteur_t* lr){
  lr->mutex = malloc(sizeof(pthread_mutex_t));
  lr->nbLs = malloc(sizeof(int));
  lr->nbLa = malloc(sizeof(int));
  lr->enEcriture = malloc(sizeof(int));
  lr->f = malloc(sizeof(File));
  pthread_mutex_init(lr->mutex, NULL);
  *lr->nbLs = 0;
  *lr->nbLa = 0;
  *lr->enEcriture = 0;
  init_file(lr->f);
}

/*
  detruire_lecteur_redacteur:
  - Spécification: prends en paramètre l'adresse d'une structure de données de type lecteur_redacteur_t
  - Sémantique: libère les emplacements mémoire des attributs de la structure.
*/
void detruire_lecteur_redacteur(lecteur_redacteur_t* lr){
  free(lr->mutex);
  free(lr->nbLs);
  free(lr->nbLa);
  free(lr->enEcriture);
  detruire_file(lr->f);
}

/*
  debut_redaction:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un rédacteur tente d'écrire.
  - Implémentation:
    Met en place un verrou pour garantir l'accès exclusif aux ressources globales de la structure lecteur_redacteur_t.
    Si des lecteurs sont actifs (i.e nbLa > 0) ou si un rédacteur est actif, on met le rédacteur courant en sommeil dans la file d'attente et on relâche le verrou en attendant d'être réveillé.
    A son réveil si aucun lecteur n'est actif et aucun rédacteur n'est actif le
    rédacteur courant devient actif.
*/
void debut_redaction(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  pthread_cond_t* cond = ajouter_redacteur_file(lr->f,(int) pthread_self());
  while((*lr->nbLa) > 0 || (*lr->enEcriture)) {
    pthread_cond_wait(cond, lr->mutex);
  }
  (*lr->enEcriture) = 1;
  supprimer_element(lr->f, (int) pthread_self());
  pthread_mutex_unlock(lr->mutex);
}

/*
  fin_redaction:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un rédacteur a fini d'écrire.
    Un rédacteur ayant fini d'écrire va regarder la file de threads en attente.
    Si le prochain fil à réveiller est un lecteur on le réveille ainsi que tous les lecteurs
    de la file jusqu'au prochain rédacteur en attente.
    Sinon on reveille le rédacteur en tête de file.
  - Implémentation:
    Met en place un verrou pour garantir l'accès exclusif aux ressources globales de la structure lecteur_redacteur_t.
    Si la fille d'attente n'est pas vide:
    Envoie des signaux de reveil aux lecteurs qui se suivent en tête de file d'attente ou bien
    réveille le rédacteur en tête de la file.

  - Exemple1:
    file_attente = [L1, L2, L3, L4, R]
    on réveille L1, L2, L3 ,L4.

  - Exemple2:
    file_attente = [R1, L1, L2, R2]
    On reveille R1.
*/
void fin_redaction(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  *lr-> enEcriture = 0;
  Cel* iterator = malloc(sizeof(Cel));
  iterator = lr->f->head;
  if(iterator != NULL) {
    if(*iterator->type == 1) {
      pthread_cond_signal(iterator->cond);
      } else {
      while (iterator != NULL) {
        if(*iterator->type == 0) {
          pthread_cond_signal(iterator->cond);
        } else {
          break;
        }
        iterator = iterator->next;
      }
    }
  }
  pthread_mutex_unlock(lr->mutex);
}

/*
  debut_lecture:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un lecteur essaye de commencer sa lecture.
  - Implémentation:
    Met en place un verrou pour garantir l'accès exclusif aux ressources globales de la structure lecteur_redacteur_t.
    Si un rédacteur est actif on met le lecteur courant en sommeil et on le place dans la file d'attente.
    Le verrou est relâché pendant le sommeil du lecteur.
    Lorsqu' il est réveillé et si aucun rédacteur n'est actif, le lecteur courant devient actif.
    Met à jour nbLs et nbLa, le nombre de lecteurs en sommeil et actifs respectivement.
*/
void debut_lecture(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  (*lr->nbLs)++;
  pthread_cond_t* cond = ajouter_lecteur_file(lr->f, (int) pthread_self());
  while(*lr->enEcriture) {
    pthread_cond_wait(cond, lr->mutex);
  }
  (*lr->nbLa)++;
  (*lr->nbLs)--;
  supprimer_element(lr->f, (int) pthread_self());
  pthread_mutex_unlock(lr->mutex);
}

/*
  fin_lecture:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un lecteur finit sa lecture.
  - Implémentation:
    Met en place un verrou pour garantir l'accès exclusif aux ressources globales de la structure lecteur_redacteur_t.
  - Assertion: nbLs == 0
    Un lecteur n'a jamais besoin de réveiller un autre lecteur.
    En effet, un lecteur peut être actif dans deux situations:
    - Il est arrivé avant le tout premier rédacteur.
    - Il a été réveillé par un rédacteur ayant fini son écriture.
    Dans le premier cas, comme aucun rédacteur n'est encore entré, aucun lecteur ne peut être endormi.
    Dans le second cas, les lecteurs endormis dans file ont reçu un signal de réveil et si il existe, le prochain fil d'exécution à réveiller est un rédacteur.
    Un lecteur n'a donc jamais besoin de réveiller un autre lecteur.
    Aussi lorsqu'il a fini sa lecture le dernier lecteur actif signalera au premier fil d'exécution en attente dans la file qu'il peut se réveiller. Ce fil, si il existe, sera nécessairement un rédacteur.
*/
void fin_lecture(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  (*lr->nbLa)--;
  if(lr->f->head != NULL){
    if(*lr->nbLa == 0){
      pthread_cond_signal(lr->f->head->cond);
    }
  }
  pthread_mutex_unlock(lr->mutex);
}
