/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP5:
  Borne Jonathan

  Solution au problème des lecteurs-rédacteurs avec priorité aux lecteurs.
  - Énoncé général:
    Dans ce problème, des lecteurs et des rédacteurs souhaitent accéder de manière
    concurrente à une donnée.
    On n'autorise qu'un seul rédacteur à écrire la donnée à la fois.
    On autorise plusieurs lecteurs à lire la donnée en même temps.
    Lecteurs et rédacteurs ne peuvent pas accéder à la donnée simultanément.

    Chaque lecteur et rédacteur est modélisé par un fil d'exécution.
    Un fil peut être dans l'un des deux états {Actif, Sommeil}.
    On définit une file d'attente fileL dans laquelle on place les Lecteurs en sommeil et
    une file d'attente fileR dans laquelle on place les Rédacteurs en sommeil.

  - Priorité aux lecteurs:
    On maintient dans deux variables le nombre de lecteurs actifs et le nombre de lecteurs
    en sommeil.
    A la fin de sa rédaction, si un rédacteur voit que des lecteurs attendent, il les réveille
    tous.

  - Sémantique des variables:
    - L'entier nbLs compte le nombre de lecteurs en sommeil.
    - L'entier nbLa compte le nombre de lecteurs actifs.
    - L'entier enEcriture vaut 1 si un rédacteur est actif 0 sinon.
    - fileL est une file d'attente dans laquelle on place les lecteurs en sommeil.
    - fileR est une file d'attente dans laquelle on place les rédacteurs en sommeil.
    - mutex est un verrou que l'on peut placer pour garantir un accès exclusif à la
    structure lecteur_redacteur_t.
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "lecteur_redacteur.h"

/*
  initialiser_lecteur_redacteur:
  - Spécification: prends en paramètre l'adresse d'une structure de données de type
  lecteur_redacteur_t
  - Sémantique: initialise les attributs de la structure.
*/
void initialiser_lecteur_redacteur(lecteur_redacteur_t* lr){
  lr->mutex = malloc(sizeof(pthread_mutex_t));
  lr->fileL = malloc(sizeof(pthread_cond_t));
  lr->fileR = malloc(sizeof(pthread_cond_t));
  lr->nbLs = malloc(sizeof(int));
  lr->nbLa = malloc(sizeof(int));
  lr->enEcriture = malloc(sizeof(int));
  pthread_mutex_init(lr->mutex, NULL);
  pthread_cond_init(lr->fileL, NULL);
  pthread_cond_init(lr->fileR, NULL);
  *lr->nbLs = 0;
  *lr->nbLa = 0;
  *lr->enEcriture = 0;
}

/*
  detruire_lecteur_redacteur:
  - Spécification: prends en paramètre l'adresse d'une structure de données de type
    lecteur_redacteur_t.
  - Sémantique: libère les emplacements mémoire des attributs de la structure.
*/
void detruire_lecteur_redacteur(lecteur_redacteur_t* lr){
  free(lr->mutex);
  free(lr->fileL);
  free(lr->fileR);
  free(lr->nbLs);
  free(lr->nbLa);
  free(lr->enEcriture);
}

/*
  debut_redaction:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un rédacteur tente d'écrire.
  - Implémentation: Met en place un verrou pour garantir l'accès exclusif aux ressources
    globales de la structure lecteur_redacteur_t. Si des lecteurs sont actifs (i.e nbLa > 0)
    ou si un rédacteur est actif, on met le rédacteur courant en sommeil, on l'ajoute
    dans la file d'attente fileR et on relâche le verrou en attendant d'être réveillé.
    A son réveil si aucun lecteur n'est actif et aucun rédacteur n'est actif le rédacteur
    courant devient actif.
*/
void debut_redaction(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  while((*lr->nbLa) > 0 || (*lr->enEcriture)) {
    pthread_cond_wait(lr->fileR, lr->mutex);
  }
  (*lr->enEcriture) = 1;
  pthread_mutex_unlock(lr->mutex);
}

/*
  fin_redaction:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un rédacteur a fini d'écrire.
    Un rédacteur ayant fini d'écrire va essayer de donner la main au lecteurs en attente, si ils existent.
  - Implémentation: Met en place un verrou pour garantir l'accès exclusif aux ressources
    globales de la structure lecteur_redacteur_t. Si des lecteurs sont en attente dans
    fileL (i.e nbLs > 0) on les réveille tous. Si plus aucun lecteur n'est en attente on
    signale un des rédacteurs (potentiellement) en attente qu'il peut commencer à rédiger.
*/
void fin_redaction(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  *lr-> enEcriture = 0;
  if((*lr->nbLs) == 0) {
    pthread_cond_signal(lr->fileR);
  } else {
    pthread_cond_broadcast(lr->fileL);
  }
  pthread_mutex_unlock(lr->mutex);
}

/*
  debut_lecture:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un lecteur essaye de commencer sa lecture.
  - Implémentation: Met en place un verrou pour garantir l'accès exclusif aux ressources
    globales de la structure lecteur_redacteur_t.
    Si un rédacteur est actif on met le lecteur courant en sommeil et on le place dans
    la file d'attente fileL. Le verrou est relâché pendant le sommeil du lecteur.
    A son réveil, si aucun rédacteur n'est actif, le lecteur courant devient actif.
    Met à jour nbLs et nbLa, le nombre de lecteurs en sommeil et actifs respectivement.
*/
void debut_lecture(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  (*lr->nbLs)++;
  while(*lr->enEcriture) {
    pthread_cond_wait(lr->fileL, lr->mutex);
  }
  (*lr->nbLs)--;
  (*lr->nbLa)++;
  pthread_mutex_unlock(lr->mutex);
}

/*
  fin_lecture:
  - Spécification: Prends en paramètre l'adresse d'une structure de type lecteur_redacteur_t
  - Sémantique: Cette procédure est appelée lorsqu'un lecteur finit sa lecture.
    Le dernier lecteur actif signale un des rédacteurs potentiellement en attente
    dans fileR qu'il peut se réveiller.
  - Implémentation: Met en place un verrou pour garantir l'accès exclusif
    aux ressources globales de la structure lecteur_redacteur_t.
  - Assertion: nbLs == 0
    On veut montrer qu'un lecteur n'a jamais besoin de réveiller un autre lecteur.
    En effet, un lecteur peut être actif dans deux situations:
    - Il est arrivé avant le tout premier rédacteur.
    - Il a été réveillé par un signal broadcast à listL envoyé par un rédacteur
      ayant fini son écriture.
    Dans le premier cas, comme aucun rédacteur n'est encore entré,
    aucun lecteur ne peut être endormi.
    Dans le second cas, les lecteurs endormis dans fileL ont reçu un signal
    de réveil broadcast, ils redeviennent tous actifs et fileL est vide.
    Cela signifie qu'un lecteur n'a jamais besoin de réveiller un autre lecteur.
*/
void fin_lecture(lecteur_redacteur_t* lr) {
  pthread_mutex_lock(lr->mutex);
  (*lr->nbLa)--;
  if((*lr->nbLa) == 0) {
    pthread_cond_signal(lr->fileR);
  }
  pthread_mutex_unlock(lr->mutex);
}
