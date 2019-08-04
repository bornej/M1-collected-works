/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP8:
  Borne Jonathan
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "stdes.h"

/* utilitaires */
int min(int a, int b) {
  return ((a<b)?a:b);
}


void print_buffer(FICHIER* f) {
  for (int i = 0; i< f->nb_octets_buffer; i++) {
    printf("%c", f->buffer[i]);
  }
  printf("\n");
}

/*
  ouvrir:
  - Spécification: FICHIER* ouvrir(char *nom, char mode)
      Prends en paramètre un nom de fichier "nom" et un caractère "mode",
      retourne l'adresse d'une structure FICHIER.
  - Sémantique:
      La fonction ouvrir, utilise l'appel système open pour ouvrir le fichier dont le nom
      est fourni en paramètre et initie un flux de données en écriture ou en lecture
      vers/depuis le fichier.
      La nature du flux est spécifiée par le paramètre mode qui peut prendre les valeurs:
        'R' pour un flux en lecture.
        'W' pour un flux en écriture.
      La fonction initialise un structure FICHIER contenant:
        Le descripteur du fichier retourné par l'appel système open
        Un tampon destiné a envoyer/recevoir des données vers/depuis
        le fichier par blocs d'octet.

      La fonction retourne NULL si l'ouverture de fichier a échoué ou si le mode
      est invalide.
*/
FICHIER* ouvrir(char *nom, char mode) {
  int descriptor = -1;
  FICHIER *f = NULL;
  if (mode == 'R') {
    descriptor = open(nom, O_RDONLY);
  } else if( mode == 'W') {
    descriptor = open(nom, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IROTH);
  } else {
    /* Si mode n'est pas égal à "R" ou "W" on retourne NULL */
    printf("Mode incorrect\n");
    return NULL;
  }

  /* si descriptor == -1, l'ouverture du fichier a échoué et on retourne NULL */
  if (descriptor == -1) {
    printf("erreur lors de l'appel open() %s\n", strerror(errno));
    return NULL;
  } else {
    f = malloc(sizeof(FICHIER));
    f->mode = mode;
    f->file = descriptor;
    f->offset = 0;
    f->global_offset = 0;
    f->nb_octets_buffer = 0;
    f->nb_octets_buffer = 0;
  }
  return f;
}

/*
  fermer:
  - Spécification: int fermer(FICHIER* f)
      Prends en paramètre l'adresse d'une structure FICHIER.
  - Sémantique:
      La fonction fermer, utilise l'appel système close pour fermer le fichier dont le
      descripteur appartient à la structure f. En cas d'echec la fonction retourne -1.
  - Pré_condition:
      le paramètre f est un pointeur vers une structure FICHIER retournée par
      la fonction ouvrir.
  - Post-condition:
      Le fichier dont le descripteur est f->file est fermé.
      L'espace mémoire de la structure FICHIER pointée par f est libérée.
*/
int fermer(FICHIER*f) {
  int res = -1;
  if (f == NULL){
    return -1;
  } else {
    if (f->mode == 'W') {
      write(f->file, f->buffer, f->nb_octets_buffer);
    }
    res = close(f->file);
    free(f);
  }
  return res;
}

/*
  lire:
  - Spécification:
      int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)
      Prends en paramètre:
        - Une adresse p où stocker les éléments lus.
        - Un entier positif "taille" qui détermine la taille d'un élément à lire
          en nombre d'octets.
        - un entier positif nbelem qui correspond au nombre d'éléments à lire depuis
          le fichier.
        - L'adresse d'une structure FICHIER.
  - Sémantique:
      La fonction lire, lis un nombre d'éléments nbelem depuis le fichier dont le descripteur
      appartient à la structure f fournie en paramètre.
      Un élément est un 'bloc' d'octet dont la taille est spécifiée par le paramètre
      taille.
      Les éléments lus sont stockés à l'adresse p fournie en paramètre.
      Retourne le nombre d'éléments lus en cas de succès, -1 en cas d'échec.
      - Remarque :
      Si nbelem*taille > taille_fichier (en octets).
      On lit le nombre maximum possible d'éléments que le fichier peut contenir.

  - Mise en oeuvre:
      On calcule le nombre total d'octets que l'on doit lire dans le fichier.
      On itère l'opération suivante.
      Tant que l'on a pas lu nb_total_octets_a_lire:
        On remplit le buffer de la structure FICHIER passée en paramètre
        avec un appel read(File, buffer, BUFFER_SIZE) puis on procède à la lecture des
        octets depuis ce buffer.
  - Pré_condition:
      Le paramètre f est un pointeur vers une structure FICHIER valide retournée par
      la fonction ouvrir avec le mode 'R' (lecture).
      Suffisament d'espace doit être disponible à l'adresse p pour stocker les données lues.
  - Post-condition:
      Les nbelem éléments lus depuis le fichier
      décrit par le descripteur f->file sont stockés à l'adresse p.
  - Valeur retour:
      Nombre d'éléments lus ou -1 si echec lors de la lecture.

  - Illustrations:
  Exemple 1: Lectures successives de 16 octets dans un fichier:
  On ouvre le fichier File avec ouvrir().
  On appelle la fonction lire(p, 1, 16, File) une première fois.

  +--              buffer               --+
  |---------------------------------------|-----------...-----------+
  |<-           BUFFER_SIZE             ->|                         | File
  |-----------------------^---------------|-----------...-----------+
  +--                     |             --+
  ^                       |
  +--- nb_octets_a_lire --+

  a) On a (nb_octets_buffer = 0) => On remplis le buffer:
     nb_octets_buffer = Read(File, Buffer, BUFFER_SIZE)

  +--              buffer               --+
  |---------------------------------------|-----------...-----------+
  |<-         nb_octets_buffer          ->|                         | File
  |-----------------------^---------------|-----------...-----------+
  +--                     |             --+
  ^                       |               ^
  +--- nb_octets_a_lire --+               |
                                          +
                                     lseek(File, 0, SEEK_CUR)

  b) On lis les nb_octets_a_lire_buffer_courant et on les écrit en mémoire à l'adresse p:
  A l'issue de la lecture la structure FICHIER est dans l'état suivant:

  +--              buffer               --+
  |---------------------------------------|-----------...-----------+
  |///////////////////////|<-n_oct_n_lus->|                         | File
  |-----------------------^---------------|-----------...-----------+
  +--                     |             --+
                          +               ^
                    offset_buffer         |
                                          +
                                      lseek(File, 0, SEEK_CUR)

  Remarque: offset_buffer est la position du dernier octet du buffer courant lu et écrit
  en mémoire.

  c) On Appelle une nouvelle fois la fonction lire(p, 1, 16, File):

  +--              buffer               --+
  |---------------------------------------|-------<---...-----------+
  |///////////////////////|<-n_oct_n_lus->|       <                 | File
  |-----------------------^---------------|-------^---...-----------+
  +--                     |             --+       |
                          +--- nb_octets_a_lire --+

  d) On lis les nb_octets_non_lus_buffer depuis le buffer et on les stocke à l'adresse p:
  A l'issue de la lecture la structure FICHIER est dans l'état suivant:

  +--              buffer               --+
  |---------------------------------------|-------<---...-----------+
  |///////////////////////////////////////|       <                 | File
  |---------------------------------------|-------^---...-----------+
  +--                                   --+       |
                                          ^- n_o -+
                                          |
                                          +
                                    offset_buffer
  Remarque: n_o = nb_octets_a_lire

  e) On a plus aucun élément à lire dans le buffer courant donc on remplit le buffer
  avec les données suivantes du fichier.

                                          +--              buffer               --+
  +---------------------------------------|-------<---...-----------+             |
  |///////////////////////////////////////|<-  nb_octets_buffer   ->| File        |
  +---------------------------------------|-------^---...-----------+             |
                                          +--     |                 ^           --+
                                          ^- n_o -+                 |
                                          |                         +
                                          +                     lseek(File, 0, SEEK_CUR)
                                    global_offset

  Remarque: global_offset est la position du dernier octet du fichier lu et écrit
  en mémoire.

  f) On lis les n_o octets restant à lire depuis le buffer courant et on les stocke
  à l'adresse p.
  A l'issue de la lecture la structure FICHIER est dans l'état suivant:


                                          +--              buffer               --+
  +---------------------------------------|-----------...-----------+             |
  |///////////////////////////////////////|///////|<- n_oct_n_lus ->| File        |
  +---------------------------------------|-------^---...-----------+             |
                                          +--     |                             --+
                                                  +
                                            global_offset

*/
int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f) {
  if(f == NULL) {
    return -1;
    printf("structure FICHIER non initialisée\n");
  }
  void *data = p;
  int init_pos = lseek(f->file, 0, SEEK_CUR);
  int taille_fichier = lseek(f->file, 0, SEEK_END);
  /* Remmettre le curseur à sa position initiale */
  lseek(f->file, init_pos, SEEK_SET);
  int nb_max_elements_fichier = (taille_fichier-f->global_offset)/taille;
  int nb_octets_total_a_lire = min(nb_max_elements_fichier*taille, taille*nbelem);
  int nb_octets_lus = 0;
  int nb_octets_restant_a_lire = nb_octets_total_a_lire - nb_octets_lus;
  int nb_octets_a_lire_depuis_buffer_courant = 0;
  while( nb_octets_restant_a_lire > 0) {
    /* Si le buffer est vide (init) ou qu'on a lu toutes les données du buffer courant */
    /* chargement de données depuis fichier dans buffer*/
    if( f->nb_octets_buffer == 0 || f->nb_octets_non_lus_buffer == 0) {
      f->nb_octets_buffer = read(f->file, f->buffer, BUFFER_SIZE);
      f->nb_octets_non_lus_buffer = f->nb_octets_buffer;
      f->offset = 0;
    }
    nb_octets_a_lire_depuis_buffer_courant = min(nb_octets_restant_a_lire, f->nb_octets_non_lus_buffer);
    memcpy(data + nb_octets_lus, (f->buffer)+(f->offset), nb_octets_a_lire_depuis_buffer_courant);
    f->offset += nb_octets_a_lire_depuis_buffer_courant;
    f->global_offset += nb_octets_a_lire_depuis_buffer_courant;
    f->nb_octets_non_lus_buffer = f->nb_octets_buffer - f->offset;
    nb_octets_lus += nb_octets_a_lire_depuis_buffer_courant;
    nb_octets_restant_a_lire = nb_octets_total_a_lire - nb_octets_lus;
  }
  return nb_octets_lus/taille;
}

/*
  ecrire:
  - Spécification: int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)
      Prends en paramètres:
        - Une adresse p où sont stockées les données à écrire.
        - Un entier positif taille indiquant la taille en nombre d'octets d'un élément à lire.
        - Un entier positif nbelem indiquant le nombre d'éléments à lire.
        - L'adresse f d'une structure de type FICHIER.
  - Pré_condition:
      le paramètre f est un pointeur vers une structure FICHIER valide retournée par
      la fonction ouvrir avec le mode "W" (écriture).
      p pointe vers une zone mémoire contenant au moins nbelem éléments.
  - Post-condition:
      Les nbelem éléments lus à l'adresse p
      sont écrits dans le fichier dont le descripteur est f->file.
  - Valeur de retour:
      Le nombre d'éléments écrits ou -1 en cas d'échec d'écriture.
*/
int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
  if(f == NULL) {
    return -1;
    printf("structure FICHIER non initialisée\n");
  }
  char *data = (char*) p;
  int nb_octets_total_a_ecrire = taille*nbelem;
  int nb_octets_ecrits = 0;
  int nb_octets_restant_a_ecrire = nb_octets_total_a_ecrire;
  int nb_octets_a_ecrire_dans_buffer_courant = 0;

  /* Invariant: au début de la boucle le buffer n'est pas plein */
  while( nb_octets_restant_a_ecrire > 0) {
    /* On place autant d'octets qu'on peut dans le buffer */
    nb_octets_a_ecrire_dans_buffer_courant = min(nb_octets_restant_a_ecrire, f->nb_octets_non_lus_buffer);
    memcpy((f->buffer)+(f->offset), data, nb_octets_a_ecrire_dans_buffer_courant);
    f->nb_octets_buffer += nb_octets_a_ecrire_dans_buffer_courant;
    f->nb_octets_non_lus_buffer -=nb_octets_a_ecrire_dans_buffer_courant;
    f->offset += nb_octets_a_ecrire_dans_buffer_courant;
    f->global_offset += nb_octets_a_ecrire_dans_buffer_courant;
    data += nb_octets_a_ecrire_dans_buffer_courant;
    nb_octets_ecrits += nb_octets_a_ecrire_dans_buffer_courant;
    nb_octets_restant_a_ecrire -= nb_octets_a_ecrire_dans_buffer_courant;
    /* si le buffer est plein */
    if(f->nb_octets_non_lus_buffer == 0) {
      write(f->file, f->buffer, f->nb_octets_buffer);
      f->nb_octets_buffer=0;
      f->offset=0;
      f->nb_octets_non_lus_buffer=BUFFER_SIZE;
      /* on écrit son contenu en mémoire et on le vide */
    }
    /* Invariant: A la sortie de la boucle le buffer n'est pas plein
      (f->nb_octets_non_lus_buffer > 0) */
  }
  return nb_octets_ecrits/taille;
}



/*
  print_int_to_char(int n, FICHIER* fp)
  - Sémantique:
    Écrit la représentation ASCII de l'entier n dans le fichier fp.
    Retourne le nombre de caractères écrits dans le fichier.
*/
int print_int_to_char(int n, FICHIER* fp) {
  char* s = malloc(sizeof(char)*80);
  int val= n;
  int neg = 0;
  int i = 0;
  int count = 0;
  if(val == 0) {
    s[i] = '0';
    ecrire(s, sizeof(char),1,fp);
  } else {
    if (val < 0) {
      neg = 1;
      val = -val;
    }
    while (val > 9) {
      int rshift = val/10;
      int mask = rshift*10;
      int last_decimal = val - mask;
      s[i] = last_decimal + '0';
      val = rshift;
      i++;
    }
    /* A la sortie 0 < val < 10 */
    s[i] = val + '0';
    /* Si l'entier n est négatif on rajoute le signe moins */
    if( neg == 1) {
      i++;
      s[i] = '-';
    }
    for(int k = i ; k>= 0; k--) {
      ecrire(s+k, sizeof(char),1,fp);
      count++;
      //printf("%c",s[k]);
    }
    //printf("\n");
  }
  return count;
}

/*
  fecriref:
  - Spécification: int fecriref (FICHIER *fp, char *format,...)
     Prends en paramètres:
        - L'adresse f d'une structure de type FICHIER.
        - Une chaîne de caractères "format".
        - Une liste de données formatées optionnelle.
  - Chaîne "format" et paramètres optionnels:
       La chaîne format est une séquence de caractères destinée à être écrite dans le
       fichier décrit par la structure fp. Cette séquence peut contenir
       les mots spéciaux suivants: "%s", "%c", "%d". Chaque occurrence d'un de ces
       mots est substitué, lors de l'écriture dans le fichier, par la valeur d'une
       donnée passée en option et dont le type correspond au type associé au mot.
  - Types/formats des données en paramètre:
       Les types/formats des données optionnelles sont interprétés selon
       l'utilisation des marqueurs %c %s ou %d dans la chaîne de caractère "format"
       passée en paramètre.

       %c : caractère
       %s : chaîne de caractères
       %d : entier

  - Pré_conditions:
     a) Le paramètre f est un pointeur vers une structure FICHIER valide retournée par
        la fonction ouvrir avec le mode "W" (écriture).
     b) Si des données formatées sont fournies dans les paramètres optionnels:
        Il doit exister une correspondance bi-univoque entre les données formatées
        passées en paramètre et les formats spécifiés dans la chaîne format.
        En particulier:
          - Si dans la chaîne "format" on trouve n mots spéciaux "%x" (x dans {s,c,d})
            on doit avoir n données formatées en paramètre
          - L'ordre et le type des formats spécifiés dans la chaine format doit correspondre
            à l'ordre et au type des données. (voir exemples)
  - Post-condition:
     Le caractères normaux de la chaîne "format" et les données formatées
     fournies en paramètre sont écrits dans le fichier dont le descripteur est f->file.
  - Valeur de retour:
     Le nombre de caractères écrits dans le fichier.
  - Exemples d'utilisation:
     - fecriref(file, "%c", 'c') écrit dans le fichier "file" la valeur ASCII
       du caractère 'c'.
     - fecriref(file, "%s", "exemple") écrit dans le fichier "file"
       la chaine "exemple".
     - fecriref(file, "%d", 1245) écrit dans le fichier la représentation ASCII
       de l'entier 1245 i.e la chaîne de caractère "1245".
     - fecriref(file, "texte %d %s", 45, "fin texte") écrit dans "file" la chaîne texte
       suivie de la représentation ASCII de l'entier 45 suivie de la chaîne "fin texte".
     - fecriref(file, "%d %s", "texte", 45): l'ordre des données ne correspond pas
       au format spécifié -> comportement non spécifié.

  - Mise en oeuvre:
       Dans la chaîne de caractère "format" on distingue deux types de caractères.
       Les caractère normaux qui sont écrits directement dans le fichier et les
       caractères spéciaux '%' qui permettent d'insérer des données formatées
       fournies dans les arguments optionnels. On utilise l'automate d'état fini
       suivant pour lire, caractère par caractère, la chaîne "format" et traiter
       si besoin les données formatées à insérer dans le fichier:
     +-------+  +--------------------------------------+
     |'*'/(1)|  |               '%'/_nil               |
     |   +---+--+-----+                        +-------v------+
     +-->|REGULAR_CHAR|                        |    FORMAT    |
         +------^-----+                        +-------+------+
                |     'd'/(2) || 's'/(3) || 'c'/(4)    |
                +--------------------------------------+

     Légende:
       - char / (x): lit l'entrée char et éxécute l'action x.
       - -->: transitions.
       - '*': tout caractère sauf '%'.
     Actions:
       - (1): écrit le caractère courant de la chaîne "format" dans le fichier.
       - (2): récupère la prochaine donnée (qui doit être un entier)
              dans la liste des arguments optionnels et écrit sa représentation ASCII
              dans le fichier.
       - (3): récupère la prochaine donnée (qui doit être une chaîne de caractères)
              dans la liste des arguments optionnels et l'écrit dans le fichier.
       - (4): récupère la prochaine donnée (qui doit être un caractère)
              dans la liste des arguments optionnels et l'écrit dans le fichier.
*/
int fecriref (FICHIER *fp, char *format, ...) {
  va_list ap;
  int d, count;
  char c, cur, *s;
  char* fmt = format;
  count = 0;
  typedef enum {REGULAR_CHAR, FORMAT} state;

  state st = REGULAR_CHAR;
  va_start(ap, format);
  while (*fmt){
    cur = *fmt;
    switch (st) {

    case REGULAR_CHAR:
      switch (cur) {

      case '%':
        st = FORMAT;
        break;

      default:
        ecrire(&cur, sizeof(char), 1, fp);
        count++;
        st = REGULAR_CHAR;
        break;
      }

    case FORMAT:
      /* Le switch suivant est inspiré d'un exemple la page man de va_arg */
      switch (cur) {

      case 's':              /* string */
        s = va_arg(ap, char *);
        while(*s) {
          ecrire(s,sizeof(c), 1, fp);
          s++;
          count++;
        }
        st = REGULAR_CHAR;
        break;

      case 'd':              /* int */
        d = va_arg(ap, int);
        count += print_int_to_char(d, fp);
        st = REGULAR_CHAR;
        break;

      case 'c':              /* char */
        /* need a cast here since va_arg only
           takes fully promoted types */
        c = (char) va_arg(ap, int);
        ecrire(&c, sizeof(char), 1, fp);
        count++;
        st = REGULAR_CHAR;
        break;
      }
      break;
    }
    fmt++;
  }
  va_end(ap);
  return count;
}

/*
  fecrire_brutf (non utilisée):
    Similaire à la fonction fecriref mais écrit la représentation binaire
    des données formatées dans le fichier.
*/
int fecrire_brutf (FICHIER *fp, char *format, ...) {
  va_list ap;
  int d;
  char c, cur, *s;
  char* fmt = format;
  typedef enum {REGULAR_CHAR, FORMAT} state;

  state st = REGULAR_CHAR;
  va_start(ap, format);
  while (*fmt){
    cur = *fmt;
    switch (st) {

    case REGULAR_CHAR:
      switch (cur) {

      case '%':
        st = FORMAT;
        break;

      default:
        ecrire(&cur, sizeof(char), 1, fp);
        st = REGULAR_CHAR;
        break;
      }
    case FORMAT:
      switch (cur) {

      case 's':              /* string */
        s = va_arg(ap, char *);
        while(*s) {
          ecrire(s,sizeof(c), 1, fp);
          s++;
        }
        st = REGULAR_CHAR;
        break;

      case 'd':              /* int */
        d = va_arg(ap, int);
        ecrire(&d, sizeof(int), 1, fp);
        st = REGULAR_CHAR;
        break;

      case 'c':              /* char */
        c = (char) va_arg(ap, int);
        ecrire(&c, sizeof(char), 1, fp);
        st = REGULAR_CHAR;
        break;
      }
      break;
    }
    fmt++;
  }
  va_end(ap);
  return 0;
}

/*
  fliref:
  - Spécification: int fliref (FICHIER *fp, char *format,...)
    Prends en paramètres:
       - L'adresse f d'une structure de type FICHIER.
       - Une chaîne de caractères "format".
       - Une liste optionnelle de pointeurs.
  - Sémantique:
    Lis depuis le fichier représenté par la structure fp, un ensemble de données
    dont le format attendu est spécifié dans la chaîne char. Les données lues
    sont stockées aux adresses de pointeurs passés en option à la fonction fliref.

  - Chaîne "format":
       La chaîne format est une séquence de caractères décrivant le format des données
       que l'on s'attend à lire dans le fichier fp. Cette séquence peut contenir
       les mots spéciaux suivants: "%s", "%c", "%d" des caractères ASCII normaux
       et des caractères espaces ' '.

  - Types/formats des données en paramètre:
       Les types/formats des données sont interprétés par
       l'utilisation des mots %c %s ou %d dans la chaîne de caractère "format"
       passée en paramètre.

       %c : caractère -> on lit un caractère depuis le fichier.
       %s : chaîne de caractères -> on lit une séquence de caractère non ' '
       dans le fichier.
       %d : entier -> On lit un séquence de caractère décimaux depuis le fichier
       éventuellement débutée par le signe '-'.

       La présence d'un caractère espace dans la chaîne format signifie que l'on
       s'attend à lire un ou plusieurs espaces dans le fichier.

       La présence d'un caractère ASCII 'normal' signifie que l'on s'attend à lire
       le caractère en question dans le fichier.

  - Pré_conditions:
     a) Le paramètre f est un pointeur vers une structure FICHIER valide retournée par
        la fonction ouvrir avec le mode "L" (lecture).
     b) Il doit exister une correspondance bi-univoque entre les mots spéciaux (%x)
        présents dans la chaîne format et les pointeurs passés en paramètres optionnels.
        En particulier:
          - Si dans la chaîne "format" on trouve n mots spéciaux "%x" (x dans {s,c,d})
            on doit avoir n pointeurs de type correspondant en paramètre.
          - L'ordre et le type des formats spécifiés doit correspondre à l'ordre et
            au type des pointeurs passées en paramètres.
     c) L'ordre des types de données spécifiés dans la chaîne format doit correspondre
        à l'ordre dans lequel on rencontre les types de données dans le fichier.
  - Post_conditions:
        Les données formatées lues depuis le fichier sont stockées aux adresses des
        pointeurs fournis en paramètre.
  - Valeur de retour:
        Nombre de caratères lus depuis le fichier.
        -1 en cas d'échec de la lecture.
*/

int fliref (FICHIER *fp, char *format,...) {
  char cur ; /* caractère de la chaine format en cours de traitement */
  char e ; /* caractère utilisé pour lire les espaces consécutifs du fichier */
  char c; /* utilisé pour stocker le caractère du fichier en cours de traitement */
  int prochain_char_dans_e = 0;
  va_list ap;
  int count, i, res, n;
  int *d = malloc(sizeof(int));
  *d = 0;
  char *s;
  char* fmt = format;
  count = 0;
  typedef enum {REGULAR_CHAR, FORMAT} state;

  state st = REGULAR_CHAR;
  va_start(ap, format);
  while (*fmt){
    cur = *fmt;
    switch (st) {
    case REGULAR_CHAR:
      switch (cur) {

      case '%': /* lire une donnée formatée depuis le fichier */
        st = FORMAT;
        break;

      case ' ': /* lire 0..* espaces consécutifs dans le fichier */
        prochain_char_dans_e = 1;
        do {
          lire(&e,1,1,fp);
        } while (e == ' '); /*On a lu 0..* caractères ' ' dans le fichier fp.
          e contient le prochain caratère non ' ' du fichier.*/
        st = REGULAR_CHAR;
        break;

      default: /* lire un caratère normal (pas '%' ni ' ') dans le fichier */
        if(prochain_char_dans_e == 1) {
          /* On a pas besoin de lire de caractère dans le fichier,
             le caratère à traiter est dans e. */
          if(e!=cur) {
            /* Erreur: le caratère courant du fichier ne correspond pas
               au caractère spécifié par la chaine format */
            return(-1);
          } else {
            prochain_char_dans_e = 0; /* On a traité e */
          }
        } else {
          lire(&c, 1, 1, fp);
          if(c!=cur) return (-1);
        }
        st = REGULAR_CHAR;
      }
      break;

    case FORMAT:
      /* Le switch suivant est inspiré d'un exemple la page man de va_arg */
      switch (cur) {

      case 's': /* lire une séquence de caractères non ' ' depuis le fichier */
        s = va_arg(ap, char *);
        i = 0;
        /* ASSERT: s doit avoir assez de place pour stocker la séquence de caractère non
           ' ' depuis le fichier */
        if(prochain_char_dans_e == 1){
          s[i] = e;
          prochain_char_dans_e = 0;
          i++;
        }
        do {
            res = lire(&c, 1, 1, fp);
            if(c == ' '){
              s[i] = '\0';
            } else {
              s[i] = c;
            }
          i++;

        } while(c != ' ' && res != 0);
        count++;
        st = REGULAR_CHAR;
        break;

      case 'd': /* lire un entier depuis le fichier */
        d = va_arg(ap, int *);
        n = 0;
        *d = 0;
        int signe = 1;
        /* Convertir la représentation ASCII en un entier */
        /* Traitement successif des décimales de gauche à droite:
           1234 = 4+(3+(2+1*10)*10)*10 = 4+(3+(12)*10)*10 = 4+(123)*10 */
        i = 1;/*i sert au décalage à gauche (1 pour la première décimale)*/
        if(prochain_char_dans_e ==1) {
          /* Le caractère à traiter est dans e.
             Pas besoin de le lire dans le fichier */
          if( e == '-') {
            signe = -1;
          } else {
            n = e - '0';
            i = 10;
          }
          prochain_char_dans_e = 0; /* les char suivant sont dans le fichier*/
        }
        do {
          res = lire(&c, 1, 1, fp);
          if(i == 1 && c == '-') signe = -1; /*Lecture du signe*/
          if((int)c>= '0' && (int)c <= '9' && res > 0){
            /* le caractère lu est un chiffre */
            n = n * i;
            n += c - '0';
            i = 10;
            /* à l'issue de la première itération,
               i prends la valeur 10 pour le décalage à gauche
               des décimales suivantes
            */
          }
        } while ((i==1) || ((int)c>= '0' && (int)c <= '9' && res > 0));
        /* Tant que le caractère lu est un caractère décimal
           ou qu'on est à la première itération
         */
        e = c; /*On place le dernier caractère lu dans e */
        prochain_char_dans_e = 1;
        *d = signe*n; /* On place le résultat avec son signe dans d */
        count++;
        st = REGULAR_CHAR;
        break;

        case 'c': /* lire un caractère depuis le fichier */
          s = va_arg(ap, char *);

          if(prochain_char_dans_e == 1){
            s[0] = e;
            prochain_char_dans_e = 0;
            count++;
          } else {
            res = lire(&c, 1, 1, fp);
            if(c == ' '){
              s[0] = '\0';
            } else {
              s[0] = c;
            }
            count++;
          }
          st = REGULAR_CHAR;
          break;
      }
      break;
    }
    fmt++;
  }
  va_end(ap);
  return count;
}
