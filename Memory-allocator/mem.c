/*
  M1 Informatique IM2AG-UGA
  Introduction to Operating Systems - TP2:
  Borne Jonathan

  Terminologie:
  Bloc alloué -> espace contigu d'adresses utilisées.
  Zone libre -> espace contigu d'adresses non utilisées.
  Zone occupée/allouée -> Ensemble de blocs alloués.

  Préambule:
  Nous maintenons en mémoire une liste des zones libres.
  Au début de chaque zone libre se trouve une cellule contenant:
  - la taille de la zone.
  - un lien vers la cellule suivante.
  - un lien vers la cellule précédente.

  Au début de chaque bloc alloué nous sauvegardons la taille de ce bloc.
  Enfin, nous réservons au tout début de la mémoire un emplacement "protégé" contenant
  un lien vers la première cellule de la liste.
  Ces informations suffisent pour connaître à tout moment l'état de la mémoire et de la parcourir.

  Charte ASCII:

  - (*): Un pointeur

    +-+
  - |*|: Tête de la liste des zones vides.
    +-+

    +-+--------------------+
  - |s|////bloc alloué/////| : Un bloc mémoire alloué.
    +-+--------------------+
     ^
     |
     +------ En tête du bloc alloué contenant la taille du bloc

     +---------------+
   - |C = {size', *} | : Une Cellule de la liste des zones vides
     +---------------+

     +---+-------------------------+-----------------------...-----------+
   - | * | struct fb C = {size, *} |               libre                 |
     +-|-+----------------------|--+-----------------------...-----------+
       |  ^                     |
       |__|                     +---->NULL

       On représente le lien entre pointeur et l'objet pointé par une flèche.
       Pour une meilleure lisibilité nous ne représentons pas les liens arrières de la liste
       sur nos illustrations ASCII.
*/

#include "mem.h"
#include "common.h"
#include <stdio.h>

#define SIZE_T_SIZE sizeof(size_t)
#define STRUCT_FB_SIZE sizeof(struct fb)
#define STRUCT_FB_P_SIZE sizeof(struct fb*)

struct fb {
  size_t size;
  struct fb* next;
  struct fb* prev;
};

mem_fit_function_t* fit_func;

/* Utilitaires */
struct fb** get_head(){
  return ((struct fb**) get_memory_adr());
}

struct fb* get_first_cel_p(){
  struct fb** head = get_head();

  return *head;
}

size_t max(size_t x, size_t y){
  size_t res;

  (x >= y)? (res = x) : (res = y);
  return res;
}

/*
  mem_init:
  - Spécification: void mem_init(char* mem, size_t taille)
  - Sémantique:
  Initialisation de la liste des zones libres avec une seule zone:
  En tête de la mémoire se trouve struct fb** head, un pointeur vers l'adresse de C,
  le premier élément de notre liste de zones mémoires libres.

  - Exemple:
  a) Mémoire: char mem[SIZE].
  +---------------------------------------------------...-----------+
  |                           libre                                 |
  +---------------------------------------------------...-----------+
   [~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SIZE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~]

  b) Mémoire: (après mem_init(mem,SIZE))

   Head                          [~~~~~~~~~~~~~~~size ~~~~~~~~~~~~~~~]
  +---+-------------------------+-----------------------...-----------+
  | * | struct fb C = {size, *} |               libre                 |
  +-|-+----------------------|--+-----------------------...-----------+
    |  ^                     |
    |__|                     +---->NULL


  Après initialisation on a C.size = SIZE - sizeof(struct fb*) - sizeof(struct fb)
*/
void mem_init(char* mem, size_t taille) {
  struct fb** head = (struct fb**) mem;
  struct fb* firstCel = (struct fb*) (mem + sizeof(head));

  *head = firstCel;
  firstCel->size = taille - STRUCT_FB_P_SIZE - STRUCT_FB_SIZE;
  firstCel->next = NULL;
  firstCel->prev = NULL;
  mem_fit(mem_fit_first);
}

/*
  first_fit:
  - Spécification: struct fb* mem_fit_first(struct fb* currentCel, size_t size)
  - Sémantique:
  Retourne la première cellule de la liste de zones libres décrivant
  une zone capable d'allouer un espace de taille t >= size.
  Si aucune zone ne permet l'allocation la fonction retourne NULL.
*/
struct fb* mem_fit_first(struct fb* currentCel, size_t size) {
  struct fb* result;

  if (currentCel == NULL) {
    return NULL;
  } else {
    (currentCel->size >= size)? (result = currentCel) : (result = mem_fit_first(currentCel->next, size));
    return result;
  }
}

/*
  mem_alloc:
  - Spécification: void* mem_alloc(size_t size)
  - Sémantique:
  mem_alloc(size) alloue un bloc mémoire de taille t tel que: t >= max(sizeof(struct fb), size + sizeof(size_t)).
  Si aucune zone mémoire ne permet l'allocation, la fonction retourne NULL.

  Pour l'allocation d'un nouveau bloc on a deux cas possibles:

  1) Allocation à une position quelconque dans la liste:

  C.prev  Bloc mémoire
  +-+    +-------------------------+-----------...----------+-----------+
  |*| .. | struct fb C = {size, *} |          libre         |///aloué///|
  +|+    +----------------------|--+-----------...----------+-----------+
   |      ^                     |
   |______|                     +----> C.next


  a) Déplacement de C à droite du bloc alloué.

   +-+    +----------------------+----------------+---...----+-----------+
   |*| .. |/////bloc alloué//////| C = {size', *} |  libre   |///aloué///|
   +|+    +----------------------+-------------|--+---...----+-----------+
    |      ^                                   |
    |______|                                   +----> C.next

  b) Sauvegarde de la taille du bloc et mise à jour de C.prev.next

   +-+    +-+--------------------+----------------+---...----+-----------+
   |*| .. |s|////bloc alloué/////| C = {size', *} |  libre   |///aloué///|
   +|+    +-+--------------------+-------------|--+---...----+-----------+
    |       [~~~~~~ t_bloc ~~~~~] ^            |
    |_____________________________|            +----> C.next

  2) Allocation en début de liste:

   Head
   +---+-------------------------+-----------------------...------------+
   | * | struct fb C = {size, *} |               libre                  |
   +-|-+----------------------|--+-----------------------...------------+
     |  ^                     |
     |__|                     +---->C.next

  a) Déplacement de C à droite de la zone allouée .

   +-+----------------------------+---------------------------+-----...-+
   |*| ////// bloc alloué /////// | struct fb: C = {size', *} | libre   |
   +|+----------------------------+---------------------------+-----...-+
    | ^
    |_|

  b) Sauvegarde de la taille du bloc et mise à jour de la tête.

   +-+-+--------------------------+--------------------------------...-+
   |*|s|////// bloc alloué ///////| struct fb: C = {size', *} | libre  |
   +|+-+--------------------------+------------------------|-------...-+
    | [~~~~~~~~~~ t_bloc ~~~~~~~~] ^                       |
    |______________________________|                       +---> C.next
*/
void* mem_alloc(size_t size) {
  struct fb* first_cel = get_first_cel_p();
  size_t s = max(size + SIZE_T_SIZE, STRUCT_FB_SIZE);
  struct fb* current = fit_func(first_cel, s);

  if (current == NULL) {
    return NULL;
  } else {
    size_t* bloc_p = (size_t*) current;
    /* (1,2) (a) Déplacement de la cellule de zone vide à droite de la zone allouée */
    current = (struct fb*) ((char*) current + s );
    /* Mise à jour de la cellule courante */
    current->prev = ((struct fb*)bloc_p)->prev;
    current->size = ((struct fb*)bloc_p)->size - s;
    current->next = ((struct fb*)bloc_p)->next;
    /* (1,2) (b) Sauvegarde de la taille du bloc alloué en début de bloc */
    *bloc_p = s;
    if(current->prev == NULL) {
      /* (2) Mise à jour de la tête */
      struct fb** head = get_head();
      *head = current;
    } else {
      /* (1) Mise à jour de la liste */
      (current->prev)->next = current;
    }
    if(current->next != NULL)
      (current->next)->prev = current;
    /* On ne retourne pas l'en-tête du bloc alloué (d'où le +1). */
    return bloc_p+1;
  }
}

/*
  mem_free (p):
  - Précondition: p est l'adresse d'un bloc alloué retournée par mem_alloc.
  - Spécification: void mem_free(void* bloc)
  - Semantique: Libère le bloc alloué dont l'adresse p est fournie en paramètre.
  Met à jour la liste des zones libres.
  Fusionne deux cellules de liste de zones libres si elles sont adjacentes.
  TODO? vérifier l'adresse bloc avant de traiter.
  TODO factoriser les parcours de liste en fonctions externes

  - Libération:
  On observe deux cas possibles.
  1): Libération avant le début de liste (Head < p < first_cel_adr)

  Head
  +---+-+---------------+-+----------+---------------------------+-----...-+
  | * |s|  ///bloc1///  |n| /suite// | struct fb: C = {size', *} | libre   |
  +-|-+-+^--------------+-+----------+^--------------------------+-----...-+
    |  [~|~~~~ s ~~~~~~]              |
    |    p                            |
    |_________________________________|

  On déplace p en tête du bloc.

  +-+-+---------------+-+----------+---------------------------+-----...-+
  |*|s|  ///bloc1///  |n| /suite// | struct fb: C = {size', *} | libre   |
  +|+^+---------------+-+----------+^--------------------------+-----...-+
   | |                              |
   | p                              |
   |____________________  __________|

   On place à l'adresse p une nouvelle cellule de liste de zone vide et on met à jour la liste.

   +-+----------------+-+----------+---------------------------+-----...-+
   |*| struct A {s,*} |n| /suite// | struct fb: C = {s_c  , *} | libre   |
   +|+^------------|--+-+----------+^--------------------------+-----...-+
    |_|            |________________|

    *head <- A;
    A.size <- s -STRUCT_FB_SIZE; A.next <- C ; A.prev = head;
    C.prev <- A;

    Si les deux cellules A et C sont adjacentes on les fusionne (voir section Fusion Cellules).
    Comme on libère avant le début de liste, tous les espaces précédents le bloc sont forcément alloués
    -> pas de fusion arrière à considérer.

    2): Libération après le début de liste (firstcel_adr < p)
    Le bloc à libérer se trouve après la première cellule de la liste de zones vides.
    Similaire au cas 1 mais avec fusions de cellules avant et arière à considérer.

    - Fusion de cellules:

    a) Fusion avant:
    Le bloc libéré est suivi par une cellule de zone libre.

    Exemple de fusion dans le cas 1):
    +-+--------------+-+--------------------+---------------------------+-----...-+
    |*| // alloué // |s|     ///bloc1///    | struct fb: C = {s_c  , *} | libre   |
    +-+--------------+-+^-------------------+---------------------------+-----...-+
                        |
                        p

    Après libération du bloc 1 on se retrouve dans la configuration suivante:

    +-+--------------+----------------------+---------------------------+-----...-+
    |*| // alloué // | struct fb: A = {s,*} | struct fb: C = {s_c  , *} | libre   |
    +|+--------------+^------------------|--+^-----------------------|--+-----...-+
     |________________|                  |___|                       +--------->

     *head <- A;
     A.size <- s -STRUCT_FB_SIZE; A.next <- C ; A.prev = head;
     C.prev <- A;

    On fusionne les cellules A et C et on met à jour la liste;

    +-+--------------+----------------------+---------------------------------...-+
    |*| // alloué // | struct fb: A = {s,*} |              libre                  |
    +|+--------------+^------------------|--+---------------------------------...-+
     |________________|                   +------------> Next

     *head <- A;
     A.size <- s + s_c + STRUCT_FB_SIZE
     A.next <- C.next

     b) Fusion arrière:
     Si le bloc libéré est précédé d'une zone libre on fusionne la Cellule précédente
     avec la cellule A nouvellement crée. Similaire au cas de fusion avant.
*/
void mem_free(void* bloc) {
  struct fb* first_cel = get_first_cel_p();

  if (first_cel != NULL) { /* Todo: montrer que first_cel existe toujours */
    struct fb** head = get_head();
    size_t size_bloc = mem_get_size(bloc);
    char* cursor = (char*) bloc - SIZE_T_SIZE;
    struct fb* new_cel = (struct fb*) cursor;
    /*
      Cas 1 -> On libère un bloc avant le début de la liste
      Tous les espaces précédents le bloc sont forcement alloués
      -> pas de fusion arrière à considérer
    */
    if ((char*) bloc < (char*) first_cel) {
      new_cel->prev = NULL;
      new_cel->next = first_cel;
      new_cel->size = size_bloc - (size_t) STRUCT_FB_SIZE;
      first_cel->prev = new_cel;
      *head = new_cel;
      /* Fusion des cellules libres adjacentes */
      if ((char*) (new_cel->next) == (char*) new_cel + STRUCT_FB_SIZE + new_cel->size) {
        /* new_cel est suivie d'une zone libre */
        new_cel->size += STRUCT_FB_SIZE + (new_cel->next)->size;
        if((new_cel->next)->next != NULL) {
          ((new_cel->next)->next)->prev = new_cel;
          new_cel->next = ((new_cel->next)->next);
        } else {
          new_cel->next = NULL;
        }
      }
      /*
        Cas 2 -> On libère un bloc après le début de la liste
        Le bloc peut être précédé ou suivi d'une zone libre.
        -> Fusion avant et arrière à considérer.
      */
    } else {
      /* recherche de (new_cel)->prev */
      struct fb* iterator = first_cel;

      while (iterator->next != NULL && iterator->next < new_cel){
        iterator = iterator->next;
      }
      /* A la sortie, iterator pointe sur la cellule précédent le bloc libéré */
      /* Initialisation de new_cel */
      new_cel->prev = iterator;
      new_cel->size = size_bloc - (size_t) STRUCT_FB_SIZE;
      if(iterator->next == NULL) {
        /* bloc en fin de mémoire après la dernière cellule*/
        /* Todo: montrer que ce cas ne peut pas se produire */
        new_cel->next = NULL;
      } else {
        new_cel->next = iterator->next;
        /* mise à jour cellule suivante */
        (new_cel->next)->prev = new_cel;
      }
      iterator->next = new_cel;
      /* Fusion avant */
      /* new_cel est suivie d'une zone libre */
      if ((char*) (new_cel->next) == (char*) new_cel + STRUCT_FB_SIZE + new_cel->size) {
        new_cel->size += STRUCT_FB_SIZE + (new_cel->next)->size;
        if((new_cel->next)->next != NULL) {
          ((new_cel->next)->next)->prev = new_cel;
          new_cel->next = ((new_cel->next)->next);
        } else {
          new_cel->next = NULL;
        }
      }
      /* Fusion arièrre */
      /* new_cel est précédée d'une zone libre */
      if ((char*) (new_cel->prev) + STRUCT_FB_SIZE + (new_cel->prev)->size == (char*) new_cel) {
        /* new_cel est après la première cellule donc new_cel->prev existe */
        (new_cel->prev)->size += STRUCT_FB_SIZE + new_cel->size;
        if(new_cel->next != NULL) {
          ((new_cel->prev)->next) = new_cel->next;
          (new_cel->next)->prev = new_cel->prev;
        } else {
          (new_cel->prev)->next = NULL;
        }
      }
    }
  }
}


void mem_fit(mem_fit_function_t* p) {
  fit_func = p;
}

/*
  mem_get_size:
  - Spécification: size_t mem_get_size(void *zone)
  - Sémantique: Reçoit en paramètre l'adresse d'un bloc alloué
  et retourne le maximum d'octets que l'utilisateur peut stocker dans le bloc.
*/
size_t mem_get_size(void *zone) {
  if (zone == NULL)
    exit(0);
  return (*((size_t*) zone - 1));
}

/*
  parcours_blocs_alloues:
  - Spécification: void parcours_blocs_alloues (void (*print)(void *zone, size_t size, int free), char** cursor, struct fb** point_arrivee, int* libre)
  - Sémantique: Parcours tous les blocs alloués d'une zone libre et affiche leurs propriétés (adresse, taille, libre).
  Le parcours commence avec un curseur positionné sur l'en-tête d'un bloc (élément de type size_t)
  et s'arrête quand on atteint l'adresse de la zone libre suivante.
*/
void parcours_blocs_alloues (void (*print)(void *zone, size_t size, int free), char** cursor, struct fb** point_arrivee, int* libre) {
  size_t bloc_size;

  while (*cursor != ((char*) *point_arrivee)) {
    *libre = 0;
    bloc_size = *((size_t*) *cursor);
    print(*cursor, bloc_size, *libre);
    *cursor += bloc_size;
  }
}

/*
  parcours_zones_libres:
  - Propriété: La mémoire contient toujours au moins une cellule de zone libre: A monter.
  - Spécification: void parcours_zones_libres (void (*print)(void *zone, size_t size, int free), char** cursor , int* libre)
  - Sémantique: Parcours toutes les cellules de la liste des zones libres.
  Pour chaque zone la procédure affiche les propriétés (adresse, taille, libre).
  Le parcours commence avec un curseur positionné sur la première cellule de la liste (de type struct fb);
  et s'arrête quand on atteint la fin de la liste.
*/
void parcours_zones_libres (void (*print)(void *zone, size_t size, int free), char** cursor , int* libre) {
  *libre = 1;
  struct fb* point_arrivee;/* adresse de la prochaine zone libre */
  size_t bloc_size = ((struct fb*) *cursor)->size;

  print(*cursor, bloc_size, *libre); /* Affiche la cellule zone libre */
  /* Parcours de toutes les zones libres */
  while (((struct fb*) *cursor)->next != NULL) {
    /* Place point_arrivee sur la prochaine cellule de la liste */
    point_arrivee = ((struct fb*) *cursor)->next;
    /* Place curseur sur le premier bloc alloué de la zone courante */
    *cursor += STRUCT_FB_SIZE + bloc_size;
    parcours_blocs_alloues(print, cursor, &point_arrivee, libre);
    /* En sortie de parcours_blocs_alloues cur pointe vers point_arrivee */
    *libre = 1;
    bloc_size = ((struct fb*) *cursor)->size;
    print(*cursor, bloc_size, *libre); /* Affiche la cellule zone libre */
  }
}

/*
  mem_show:
  - Spécification: mem_show (void (*print)(void *zone, size_t size, int free))
  - Sémantique: Affiche l'état de la mémoire.
  - Remarque: Sachant que l'on joint les zones libres adjacentes lors de mem_free
  on a les propriétés suivantes:
  - Si elle existe la zone mémoire qui suit une zone libre est toujours une zone occupée.
  - Si elle existe la zone mémoire qui suit une zone occupée est toujours une zone libre.
  - Toute zone libre est marquée à son début par une cellule de type struct fb.
  - Tout bloc occupé est marqué à son début par sa taille.

  On parcourt la mémoire en partant de la première adresse allouable.
  Deux états de la mémoire sont possible:

  1) Zone occupée en début de mémoire (*Head != Head + 1):
  Head
  +---+----------------------------+---------------------------+-----...-+
  | * | ///// zone allouée /////// | struct fb: C = {size', *} | libre   |
  +-|-+----------------------------+---------------------------+-----...-+
    |                               ^
    |_______________________________|

  La zone allouée peut contenir plusieurs blocs.
  On place un curseur p à l'adresse (Head + 1):
  +-+-+-------------+-+----------+---------------------------+-----...-+
  |*|s| ///bloc1/// |s| /suite// | struct fb: C = {size', *} | libre   |
  +-+^+-------------+-+----------+---------------------------+-----...-+
     | [~~~~ s ~~~~]
     p

  On récupère la taille s du bloc 1 et on affiche ses informations (adresse, taille, libre)
  On passe au bloc suivant (p += s).
  +-+-+-------------+-+----------+---------------------------+-----...-+
  |*|s| ///bloc1/// |s| /suite// | struct fb: C = {size', *} | libre   |
  +-+-+-------------+^+----------+---------------------------+-----...-+
                     |
                     p

  On va parcourir tous ces blocs jusqu'à rejoindre C, la première cellule de liste des zones libres.

  2) Zone libre en début de mémoire:

  Head                           [~~~~~~~~~~~~~~ size ~~~~~~~~~~~~~~~~]
  +---+-------------------------+-----------------------...------------+
  | * | struct fb C = {size, *} |               libre                  |
  +-|-+^---------------------|--+-----------------------...------------+
    |__|                     |
       |                     +---->C.next
       p

    On à notre curseur placé sur le début d'une zone libre.
    On récupère dans C la taille size de l'espace libre, on affiche les informations de la zone (adresse, taille, libre).
    On Passe au bloc occupé suivant (p += size + sizeof(struct fb)). Une zone libre est toujours suivie d'un bloc occupé.
*/
void mem_show (void (*print)(void *zone, size_t size, int free)) {
  struct fb** head = get_head();
  struct fb* cel_destination_p = get_first_cel_p();
  int libre;
  char* cur = (char*) (head + 1); /* Place le curseur en début de mémoire allouable*/

  if((struct fb*) cur != cel_destination_p) {
    /* Cas 1) -> zone occupée en début de mémoire (peut contenir plusieurs blocs) */
    /* On parcourt tous les bloc et on rejoint la première cellule */
    parcours_blocs_alloues(print, &cur, &cel_destination_p, &libre);
    if(cur != (char*) cel_destination_p) {
      /* ASSERT: On doit être à la position de la première cellule de zone libre */
      printf("Un problème est survenu dans le parcours de la mémoire.\n");
      printf("Nous devrions être sur une cellule de zone libre.\n");
    } else {
      /* A partir d'ici on est dans le cas 2 */
      parcours_zones_libres(print, &cur, &libre);
    }
  } else {
    parcours_zones_libres(print, &cur , &libre);
  }
}
