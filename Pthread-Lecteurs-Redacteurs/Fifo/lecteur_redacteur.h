/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP5:
  Borne Jonathan
*/

#include <pthread.h>
#include "file.h"
/*
  Définition d'une structure de données correspondant à une instance
  du problème des lecteurs rédacteurs.
 */
typedef struct _lecteur_redacteur_t {
  pthread_mutex_t* mutex;
  int* nbLs; /* Nombre de lecteurs en sommeil */
  int* nbLa; /* Nombre de lecteurs actifs */
  int* enEcriture;
  File* f;
} lecteur_redacteur_t;

/*
  Initialisation de la structure de données lecteur_redacteur_t.
*/
void initialiser_lecteur_redacteur(lecteur_redacteur_t* lr);

/*
  Destruction de la structure de données lecteur_redacteur_t.
*/
void detruire_lecteur_redacteur(lecteur_redacteur_t* lr);
void debut_redaction(lecteur_redacteur_t* lr);
void fin_redaction(lecteur_redacteur_t* lr);
void debut_lecture(lecteur_redacteur_t* lr);
void fin_lecture(lecteur_redacteur_t* lr);
