#ifndef _STDES_H
#define _STDES_H

#define BUFFER_SIZE 10

struct _ES_FICHIER {
  int file;
  char mode;
  char buffer[BUFFER_SIZE];
  int nb_octets_buffer; /* nombre d'octets de données dans le buffer */
  int nb_octets_non_lus_buffer; /* nombre d'octets restant dans buffer à partir de l'offset */
  int offset; /* offset du dernier octet lu dans le buffer courant par rapport au début de ce buffer */
  int global_offset; /* offset du dernier octet lu dans le buffer courant par rapport au début du fichier */
};

typedef struct _ES_FICHIER FICHIER;

FICHIER *ouvrir(char *nom, char mode);
int fermer(FICHIER*f);
int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int fecrire_brutf (FICHIER *fp, char *format, ...);
int fecriref (FICHIER *fp, char *format, ...);
int fliref (FICHIER *fp, char *format,...);
#endif

