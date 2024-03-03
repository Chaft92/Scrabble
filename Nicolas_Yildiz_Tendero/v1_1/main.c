#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define NB_MAX_JOUEURS 4 //nombre maximum de joueur
#define NB_MIN_JOUEURS 2 //nombre minimum de joueur
#define TAILLE_MAX_NOM 10 //taille maximum des noms des joueurs
#define NB_MAX_LETTRES_CHEVALET 7 //nombre de lettres dans le chevalet
#define NB_LETTRES 27 // nombre de lettres utilisees
#define NB_JETONS 102 //nombre de lettres dans la pioche
#define TAILLE_PLATEAU 15 // taille du jeu de scrabble
#define NB_MELANGE 1000 // nombre de fois qu'on melange les lettres de la pioche
#define POINTS_SCRABBLE 50 // nb de points en plus si pose de 7 lettres

//structure lettre
typedef struct lettre {
    char lettre; //lettre
    int nbPoints; //nombre de points de la lettre
    int nbOccurrences; //nombre d occurrence de la lettre
} Lettre;


//structure chevalet
typedef struct chevalet {
    int nbLettres; //nombre de lettre sur le chevalet
    char lettres[NB_MAX_LETTRES_CHEVALET]; //lettres du chevalet
} Chevalet;


//structure joueur
typedef struct joueur {
    char nom[TAILLE_MAX_NOM]; //nom du joueur
    int score; //score du joueur = points marques
    Chevalet chevalet; //chevalet du joueur
} Joueur;

//structure pioche
typedef struct pioche {
    char jetons[NB_JETONS]; //jetons
    int nbJetons; //nombre de jetons restant dans la pioche
} Pioche;

//structure plateau
typedef struct plateau {
    char cases[TAILLE_PLATEAU][TAILLE_PLATEAU]; //cases du plateau
    char bonus[TAILLE_PLATEAU][TAILLE_PLATEAU]; // bonus des cases du plateau
} Plateau;

//structure partie
typedef struct partie {
    int nbJoueurs; // nb de joueurs
    Joueur joueurs[NB_MAX_JOUEURS]; // joueurs
    Plateau plateau; // plateau
    Pioche pioche; // pioche
    int tour; // numero du tour a partir de 0
    int numJoueur; // numero du joueur courant
    int fin; // indicateur fin de partie
} Partie;

//structure coup
typedef struct coup {
    int ligne; // ligne de la premiere lettre du mot principal entre 1 et 15
    int colonne; // colonne de la premiere lettre du mot principal entre 1 et 15
    char lettres[NB_MAX_LETTRES_CHEVALET+1]; // lettres posees
    char direction; // direction V ou H du mot principal
} Coup;

//structure mot
typedef struct mot {
    int ligne; // ligne de la premiere lettre du mot entre 1 et 15
    int colonne; // colonne de la premiere lettre du mot entre 1 et 15
    char mot[TAILLE_PLATEAU]; // mot
    char direction;// direction V ou H du mot
} Mot;

/**
 * attente saisie espace
 */
void attenteEspace () {
    printf("\n\nAppuyez sur espace pour retourner au menu."); // action a faire
    char choix = 'N';
    do {
        choix = fgetchar();
    } while (choix != ' ');
}

/**
 * affichage des regles du jeu
 */
void afficherRegles () {//sous programme des regles
    system("cls"); // effacement ecran
    printf("Regles du jeu de scrabble\n\n" // regles du scrabble
           "\"Chaque joueur joue a tour de role. Celui qui commence sera celui qui obtiendra le plus petit nombre aleatoire (de 1 a 6). \n\n"
           "           \"En cas d egalite du plus petit nombre avec un (ou plusieurs) autre(s) joueur(s), on prendra celui qui a un nom alphabetiquement avant celui des autres ex aequo au tirage aleatoire.\n\n"
           "           \"Le joueur choisit parmi les lettres qu il a tirees et pose une ou plusieurs lettres de maniere a former de nouveaux mots sur la grille, horizontalement (de la gauche vers la droite) ou verticalement (de haut en bas).\n\n"
           "           \"Il n'est pas admis de poser un mot en diagonale. Le premier mot doit faire au moins deux lettres et recouvrir la case centrale (materialisee par le caractere #) et les mots suivants doivent s appuyer sur des mots deja places, a la maniere des mots croises. \n\n"
           "           \"Le score d'un coup est calcule en additionnant la valeur de toutes les lettres des nouveaux mots formes (y compris celles deja posees sur la grille).\n\n"
           "           \"Si une nouvelle lettre posee recouvre une case lettre compte double  (caractere &) ou lettre compte triple (caractere \%), sa valeur est multipliee par 2 ou par 3 ; si cette case fait partie de 2 nouveaux mots formes, horizontalement et verticalement, elle compte double ou triple dans les deux sens. \n\n"
           "           \"Si une case mot compte double (caractere @) ou mot compte triple (caractere %c) a ete recouverte, la valeur du mot entier est doublee ou triplee. \n \n"
           "           \"Si en recouvrant cette case, cela forme deux mots horizontalement et verticalement, la valeur des deux mots est multipliee par 2 ou par 3. Les cases multiplicatrices (concernant les mots) ne servent plus une fois recouvertes. \n\n"
           "           \n\n"
           "           \"Placer ses sept lettres sur la grille apporte une prime de 50 points. Ce genre de coup est appele scrabble en francais. Le joueur qui fait un scrabble, a scrabble. \n\n"
           "           \"Apres avoir pose ses lettres et annonce son score, le tour est termine. Il est alors possible de demander a verifier la presence d'un mot joue dans un dictionnaire de reference, convenu entre les joueurs : \n\n"
           "           \"-----> Sans bonus, la verification est manuelle \n\n"
           "           \"-----> Avec le bonus, le programme verifiera lui-meme la validite du mot tel que defini plus bas.. \n\n"
           "           \"Si un mot conteste n'est pas valable, le joueur reprend ses lettres et marque zero point pour le coup. Par contre si le mot conteste est valable, le demandeur qui a conteste perd dix points. \n\n"
           "           \n\n"
           "           \"Les actions possibles sont : \n\n"
           "           \"----> Passer son tour \n\n"
           "           \"----> Changer une ou plusieurs lettres de son jeu (dans ce cas il est impossible de poser un nouveau mot). \n\n"
           "           \n\n"
           "           \"La partie s acheve quand le sac est vide et qu un joueur place ses dernieres lettres sur la grille. Il ajoute a son score la valeur des lettres restant a ses adversaires, qui dans le meme temps, doivent deduire de leur score la valeur des lettres qui leur restent.\n\n"
           "           \n\n"
           "           \n\n"
           "           \"Le vainqueur est celui qui totalise le plus de points a la fin de la partie. \n\n", 21);
    // regles
    attenteEspace();
}

/**
 * methode de creation de toutes les lettres avec leurs points et leurs occurences
 *
 * @param ls pointeur sur la liste des lettres
*/
void creerLettres (Lettre* ls) {

    //  lettres et symbole pour les bonus de points
    ls[0].lettre = 'A';
    ls[0].nbPoints = 1;
    ls[0].nbOccurrences = 9;

    ls[1].lettre = 'B';
    ls[1].nbPoints = 3;
    ls[1].nbOccurrences = 2;

    ls[2].lettre = 'C';
    ls[2].nbPoints = 3;
    ls[2].nbOccurrences = 2;

    ls[3].lettre = 'D';
    ls[3].nbPoints = 2;
    ls[3].nbOccurrences = 3;

    ls[4].lettre = 'E';
    ls[4].nbPoints = 1;
    ls[4].nbOccurrences = 15;

    ls[5].lettre = 'F';
    ls[5].nbPoints = 4;
    ls[5].nbOccurrences = 2;

    ls[6].lettre = 'G';
    ls[6].nbPoints = 2;
    ls[6].nbOccurrences = 2;

    ls[7].lettre = 'H';
    ls[7].nbPoints = 4;
    ls[7].nbOccurrences = 2;

    ls[8].lettre = 'I';
    ls[8].nbPoints = 1;
    ls[8].nbOccurrences = 8;

    ls[9].lettre = 'J';
    ls[9].nbPoints = 8;
    ls[9].nbOccurrences = 1;

    ls[10].lettre = 'K';
    ls[10].nbPoints = 10;
    ls[10].nbOccurrences = 1;

    ls[11].lettre = 'L';
    ls[11].nbPoints = 1;
    ls[11].nbOccurrences = 5;

    ls[12].lettre = 'M';
    ls[12].nbPoints = 2;
    ls[12].nbOccurrences = 3;

    ls[13].lettre = 'N';
    ls[13].nbPoints = 1;
    ls[13].nbOccurrences = 6;

    ls[14].lettre = 'O';
    ls[14].nbPoints = 1;
    ls[14].nbOccurrences = 6;

    ls[15].lettre = 'P';
    ls[15].nbPoints = 3;
    ls[15].nbOccurrences = 2;

    ls[16].lettre = 'Q';
    ls[16].nbPoints = 8;
    ls[16].nbOccurrences = 1;

    ls[17].lettre = 'R';
    ls[17].nbPoints = 1;
    ls[17].nbOccurrences = 6;

    ls[18].lettre = 'S';
    ls[18].nbPoints = 1;
    ls[18].nbOccurrences = 6;

    ls[19].lettre = 'T';
    ls[19].nbPoints = 1;
    ls[19].nbOccurrences = 6;

    ls[20].lettre = 'U';
    ls[20].nbPoints = 1;
    ls[20].nbOccurrences = 6;

    ls[21].lettre = 'V';
    ls[21].nbPoints = 4;
    ls[21].nbOccurrences = 2;

    ls[22].lettre = 'W';
    ls[22].nbPoints = 10;
    ls[22].nbOccurrences = 1;

    ls[23].lettre = 'X';
    ls[23].nbPoints = 10;
    ls[23].nbOccurrences = 1;

    ls[24].lettre = 'Y';
    ls[24].nbPoints = 10;
    ls[24].nbOccurrences = 1;

    ls[25].lettre = 'Z';
    ls[25].nbPoints = 10;
    ls[25].nbOccurrences = 1;

    ls[26].lettre = 63;
    ls[26].nbPoints = 0;
    ls[26].nbOccurrences = 2;
}

/**
 * sous programme affichant une lettre avec ces points et le nombre d occurence de la lettre
 *
 * @param le lettre a afficher
 */
void voirLettre (Lettre le) {
    printf("%c %d %d\n",le.lettre, le.nbPoints, le.nbOccurrences);
}

/**
 * affichage de toutes les lettres avec points et nombre
 *
 * @param ls pointeur sur les lettres
 * @param nb nb de lettres
 */
void voirLettres (Lettre* ls, int nb) {
    for (int i = 0; i < nb; ++i) {
        voirLettre(ls[i]);
    }
}

/**
 * sous programme qui cree la pioche
 *
 * @param ls pointeur sur la liste des lettres
 * @param nb nb de lettres
 * @return la pioche
 */
Pioche creerPioche (Lettre* ls, int nb) {
    Pioche p;
    int k=0;
    p.nbJetons = 0;
    for (int i = 0; i<nb; i++) {
        for (int j = 0; j < ls[i].nbOccurrences; ++j) {
            p.jetons[k] = ls[i].lettre;
            p.nbJetons++;
            k++;
        }
    }
    return p;
}

/**
 * sous programme pour afficher la pioche et le nb de jetons dans la pioche
 * @param p la pioche
 * @param detail 0 affichage du  nb de jetons uniquement, 1 affichage du contenu de la pioche
 */
void voirPioche (Pioche p, int detail) {
    printf("pioche : nb jetons = %d\n", p.nbJetons);
    if (detail == 1) {
        for (int i = 0; i < p.nbJetons; ++i) {
            printf("%c ", p.jetons[i]);
        }
        printf("\n");
    }
}

/**
 * sous programme qui interverit 2 lettres de la pioche (utilisation plusieurs fois pour melanger la pioche)
 *
 * @param p pointeur vers la pioche
 */
void melangerPioche (Pioche *p) {
    int i1 = rand()%(p->nbJetons);
    int i2 = rand()%(p->nbJetons);
    char temp = p->jetons[i1];
    p->jetons[i1] = p->jetons[i2];
    p->jetons[i2] = temp;
}

/**
 * sous programme qui cree le plateau de jeu avec les bonus (sans aucun jeton)
 *
 * @return pl le plateau avec les bonus
 */
Plateau creerPlateau () {
    Plateau pl;
    for (int i = 0; i < TAILLE_PLATEAU; ++i) {
        for (int j = 0; j < TAILLE_PLATEAU; ++j) {
            pl.cases[i][j] = ' ';
        }
    }
    for (int i = 0; i < TAILLE_PLATEAU; ++i) {
        for (int j = 0; j < TAILLE_PLATEAU; ++j) {
            pl.bonus[i][j] = ' ';
        }
    }
    pl.bonus[7][7] = '#';
    pl.bonus[0][0] = 21;
    pl.bonus[0][7] = 21;
    pl.bonus[0][14] = 21;
    pl.bonus[7][0] = 21;
    pl.bonus[7][14] = 21;
    pl.bonus[14][0] = 21;
    pl.bonus[14][7] = 21;
    pl.bonus[14][14] = 21;
    pl.bonus[0][3] = '&';
    pl.bonus[0][11] = '&';
    pl.bonus[2][6] = '&';
    pl.bonus[2][8] = '&';
    pl.bonus[3][0] = '&';
    pl.bonus[3][7] = '&';
    pl.bonus[3][14] = '&';
    pl.bonus[6][2] = '&';
    pl.bonus[6][6] = '&';
    pl.bonus[6][8] = '&';
    pl.bonus[6][12] = '&';
    pl.bonus[7][3] = '&';
    pl.bonus[7][11] = '&';
    pl.bonus[8][2] = '&';
    pl.bonus[8][6] = '&';
    pl.bonus[8][8] = '&';
    pl.bonus[8][12] = '&';
    pl.bonus[11][0] = '&';
    pl.bonus[11][7] = '&';
    pl.bonus[11][14] = '&';
    pl.bonus[12][6] = '&';
    pl.bonus[12][8] = '&';
    pl.bonus[14][3] = '&';
    pl.bonus[14][11] = '&';
    pl.bonus[1][1] = '@';
    pl.bonus[1][13] = '@';
    pl.bonus[2][2] = '@';
    pl.bonus[2][12] = '@';
    pl.bonus[3][3] = '@';
    pl.bonus[3][11] = '@';
    pl.bonus[4][4] = '@';
    pl.bonus[4][10] = '@';
    pl.bonus[10][4] = '@';
    pl.bonus[10][10] = '@';
    pl.bonus[11][3] = '@';
    pl.bonus[11][11] = '@';
    pl.bonus[12][2] = '@';
    pl.bonus[12][12] = '@';
    pl.bonus[13][1] = '@';
    pl.bonus[13][13] = '@';
    pl.bonus[1][5] = '%';
    pl.bonus[1][9] = '%';
    pl.bonus[5][1] = '%';
    pl.bonus[5][5] = '%';
    pl.bonus[5][9] = '%';
    pl.bonus[5][13] = '%';
    pl.bonus[9][1] = '%';
    pl.bonus[9][5] = '%';
    pl.bonus[9][9] = '%';
    pl.bonus[9][13] = '%';
    pl.bonus[13][5] = '%';
    pl.bonus[13][9] = '%';
    return pl;
}

/**
 * sous programme qui renvoie le contenu d une case definie par ligne et colonne
 *
 * @param pl le plateau
 * @param l entier representant une ligne
 * @param c entier representant une colonne
 * @return la case du plateau (lettre ou ' ')
 */
char casePlateau (Plateau pl, int l, int c) {
    return pl.cases[l-1][c-1];
}

/**
 * sous programme qui renvoie le bonus d une case definie par ligne et colonne
 *
 * @param pl le plateau
 * @param l entier representant une ligne
 * @param c entier representant une colonne
 * @return le bonus de la case
 */
char bonusPlateau (Plateau pl, int l, int c) {
    return pl.bonus[l-1][c-1];
}

/**
 * affichage du plateau
 *
 * @param pl le plateau
 * @param bonus si 0 sans les bonus, 1 avec les bonus
 */
void afficherPlateau (Plateau pl, int bonus) {
    printf("    ");
    for (int i = 0; i < TAILLE_PLATEAU; ++i) {
        printf(" %c ", i+65);
    }
    printf("\n");
    for (int i = 1; i <= TAILLE_PLATEAU; ++i) {
        if (i<10) {
            printf(" ");
        }
        printf("%d  ", i);
        for (int j = 1; j <= TAILLE_PLATEAU; ++j) {
            if (bonus == 1) {
                printf("%c%c ", bonusPlateau(pl, i, j), casePlateau(pl, i, j));
            } else {
                printf("%c%c ", ' ', casePlateau(pl, i, j));
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

/**
 * creation d un joueur avec la saisie du nom et initialisation des parametres
 *
 * @return
 */
Joueur creerJoueur () {
    Joueur j;
    printf("quel est ton nom de joueur ?\n");
    scanf("%s", j.nom);
    fflush(stdin);
    j.score = 0;
    Chevalet c;
    c.nbLettres = 0;
    j.chevalet = c;
    return j;
}

/**
 * affichage du chevalet d un joueur
 *
 * @param c le chevalet a afficher
 */
void voirChevalet (Chevalet c) {
    if (c.nbLettres > 1) {
        printf("%d lettres dans le chevalet : ", c.nbLettres);
        for (int i = 0; i < c.nbLettres; ++i) {
            printf("%c", c.lettres[i]);
        }
        printf("\n");
    } else if (c.nbLettres == 1) {
        printf("%d lettre dans le chevalet : ", c.nbLettres);
        for (int i = 0; i < c.nbLettres; ++i) {
            printf("%c", c.lettres[i]);
        }
        printf("\n");
    } else if (c.nbLettres == 0) {
        printf("aucune lettre dans le chevalet\n");
    }
}

/**
 * affichage d un joueur
 *
 * @param j le joueur a afficher
 */
void voirJoueur (Joueur j) {
    printf("Joueur : %s\n", j.nom);
    printf("score = %d\n", j.score);
    voirChevalet (j.chevalet);
    printf("\n\n");
}

/**
 * pioche d une lettre dans la pioche (la lettre piochee est retiree de la pioche)
 *
 * @param p la pioche
 * @return la lettre piochee
 */
char piocher (Pioche *p) {
    char recup = ' ';
    if (p->nbJetons > 0) {
        recup = p->jetons[0];
        p->nbJetons--;
        for (int i = 0; i < p->nbJetons; ++i) {
            p->jetons[i] = p->jetons[i+1];
        }
    }
    return recup;
}

/**
 * remplissage du chevalet avec un maximum de 7 jetons
 *
 * @param j pointeur sur le joueur qui remplit son chevalet
 * @param p pointeur sur la pioche
 */
void remplirChevalet (Joueur *j, Pioche *p) {
    for (int i = j->chevalet.nbLettres; i < NB_MAX_LETTRES_CHEVALET; ++i) {
        char jetonPioche = piocher(p);
        if (jetonPioche != ' ') {
            j->chevalet.lettres[i] = jetonPioche;
            j->chevalet.nbLettres++;
        }
    }
}

/**
 * creation d un coup (essai de pose de lettres sur le plateau)
 *
 * @return le coup saisi par le joueur
 */
Coup creerCoup () {
    Coup c;
    do {
        printf("lettres a poser dans l ordre ?\n");
        scanf("%s", c.lettres);
        fflush(stdin);
    } while (strlen(c.lettres) == 0);
    do {
        printf("numero de la ligne de la premiere lettre du mot ?\n");
        scanf("%d", &(c.ligne));
        fflush(stdin);
    } while (c.ligne < 1 || c.ligne > TAILLE_PLATEAU);
    do {
        printf("numero de la colonne de la premiere lettre du mot ?\n");
        scanf("%d", &(c.colonne));
        fflush(stdin);
    } while (c.colonne < 1 || c.colonne > TAILLE_PLATEAU);
    do {
        printf("direction du mot H(horizontale) ou V(verticale) ?\n");
        scanf(" %c", &(c.direction));
        fflush(stdin);
    } while (c.direction != 'H' && c.direction != 'V');

    return c;
}

/**
 * creation nouvelle partie
 *
 * @param ls pointeur sur les lettres
 * @return la partie
 */
Partie creerPartie (Lettre* ls) {
    Partie part;
    part.nbJoueurs = 0;
    part.fin = 0;

    // saisir des joueurs
    do {
        printf("Nombre de joueurs ?\n");

        if (scanf("%d", &(part.nbJoueurs)) != 1) {

            printf("Erreur de saisie. Veuillez entrer un nombre valide.\n");

            while (getchar() != '\n');
            continue;
        }


        if (part.nbJoueurs < NB_MIN_JOUEURS || part.nbJoueurs > NB_MAX_JOUEURS) {
            printf("Nombre de joueurs invalide. Veuillez entrer un nombre entre %d et %d.\n", NB_MIN_JOUEURS, NB_MAX_JOUEURS);
        }

    } while (part.nbJoueurs < NB_MIN_JOUEURS || part.nbJoueurs > NB_MAX_JOUEURS);
    fflush(stdin);


    // verification que le nom est unique
    for (int i = 0; i < part.nbJoueurs; ++i) {
        int joueurExistant = 0;
        do {
            part.joueurs[i] = creerJoueur();
            joueurExistant = 0;
            for (int j = 0; j < i; ++j) {
                if (strcmp(part.joueurs[j].nom, part.joueurs[i].nom) == 0) {
                    joueurExistant = 1;
                }
            }
        } while(joueurExistant == 1);
    }

    // creation de la pioche
    part.pioche = creerPioche(ls, NB_LETTRES);
    // melange de la pioche
    for (int i = 0; i < NB_MELANGE; ++i) {
        melangerPioche(&(part.pioche));
    }

    // initialisation des chevalets des joueurs
    for (int i = 0; i < part.nbJoueurs; ++i) {
        remplirChevalet(&(part.joueurs[i]), &(part.pioche));
    }

    // creation du plateau sans jetons
    part.plateau = creerPlateau();
    // initialisation premier tour
    part.tour = 0;
    // selection aleatoire du premier joueur a jouer
    part.numJoueur = rand()%part.nbJoueurs;
    return part;
}

/**
 * teste si les lettres a poser sont dans le chevalet du joueur
 *
 * @param c chevalet du joueur
 * @param le pointeur sur lettres a poser
 * @return 0 si des lettres ne sont pas trouvees dans le chevalet, 1 si les lettres sont dans le chevalet
 */
int testerLettreChevalet (Chevalet c, char *le) {
    int lettresTrouves = 1;
    for (int i = 0; i < strlen(le); ++i) {
        int lettreTrouve = 0;
        for (int j = 0; j < c.nbLettres; ++j) {
            if (*(le+i) == c.lettres[j]) {
                lettreTrouve = 1;
                c.lettres[j] = ' ';
                break;
            }
        }
        if (lettreTrouve == 0) {
            lettresTrouves = 0;
        }
    }
    return lettresTrouves;
}

/**
 * retire du chevalet les lettres a poser
 *
 * @param c pointeur sur le chevalet
 * @param le pointeur sur les lettres a poser
 */
void retirerLettresChevalet (Chevalet *c, char *le) {
    for (int i = 0; i < strlen(le); ++i) {
        int j = 0;
        int lettreTrouvee = 0;
        do {
            if (*(le+i) == c->lettres[j]) {
                lettreTrouvee = 1;
                for (int k = j; k < c->nbLettres - 1; ++k) {
                    c->lettres[k] = c->lettres[k + 1];
                }
                c->nbLettres--;
            }
            j++;
        } while (lettreTrouvee == 0 && j < c->nbLettres);
    }

}

/**
 * ajoute des jetons a la pioche (cas echange de jetons)
 *
 * @param p pointeur sur la pioche
 * @param le pointeur sur les lettres a echanger
 */
void ajouterJetonsPioche (Pioche *p, char* le) {
    for (int i = 0; i < strlen(le); ++i) {
        p->jetons[p->nbJetons] = *(le+i);
        p->nbJetons++;
    }
}

/**
 * teste si des cases adjacentes a une case a au moins une lettre posee
 *
 * @param p le plateau
 * @param l ligne de la case a tester
 * @param c colonne de la case a tester
 * @return 1 si adjacent, 0 si aucun voisin avec lettre
 */
int adjacentRempli (Plateau p, int l, int c) {//sous programme qui regarde si un mot est asjacent a la lettre
    if (l == 1) {
        if (c == 1) {
            if (p.cases[l][c-1] != ' ' || p.cases[l-1][c] != ' ') {
                return 1;
            }
        } else if (c == TAILLE_PLATEAU) {
            if (p.cases[l][c-1] != ' ' || p.cases[l-1][c-2] != ' ') {
                return 1;
            }
        } else {
            if (p.cases[l][c-1] != ' ' || p.cases[l-1][c-2] != ' ' || p.cases[l-1][c] != ' ') {
                return 1;
            }
        }
    } else if (l == TAILLE_PLATEAU) {
        if (c == 1) {
            if (p.cases[l-2][c-1] != ' ' || p.cases[l-1][c] != ' ') {
                return 1;
            }
        } else if (c == TAILLE_PLATEAU) {
            if (p.cases[l-2][c-1] != ' ' || p.cases[l-1][c-2] != ' ') {
                return 1;
            }
        } else {
            if (p.cases[l-2][c-1] != ' ' || p.cases[l-1][c-2] != ' ' || p.cases[l-1][c] != ' ') {
                return 1;
            }
        }
    } else {
        if (c == 1) {
            if (p.cases[l-2][c-1] != ' ' || p.cases[l][c-1] != ' ' || p.cases[l-1][c] != ' ') {
                return 1;
            }
        } else if (c == TAILLE_PLATEAU) {
            if (p.cases[l-2][c-1] != ' ' || p.cases[l][c-1] != ' ' || p.cases[l-1][c-2] != ' ') {
                return 1;
            }
        } else {
            if (p.cases[l-2][c-1] != ' ' || p.cases[l][c-1] != ' ' || p.cases[l-1][c-2] != ' ' || p.cases[l-1][c] != ' ') {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * recherche du mot vertical a partir d une case ou on place une nouvelle lettre
 *
 * @param p le plateau
 * @param l ligne de la case
 * @param c  colonne de la case
 * @param lettre la lettre posee sur la case
 * @param nm pointeur sur le nouveau mot vertical trouve
 * @param nbMs pointeur sur le nb de nouveaux mots
 */
void rechercherMotVertical(Plateau  p, int l, int c, char lettre, Mot* nm, int *nbMs) {

//    printf("recherche mot vertical %d %d %c\n",l, c, lettre);
    int motCree = 0;
    int i = l;
    if (l > 1 && p.cases[l-2][c-1] != ' ') { // lettre au dessus
        motCree = 1;
        i = l-1;
    }
    if (l < TAILLE_PLATEAU && p.cases[l][c-1] != ' ') { // lettre en dessous
        motCree = 1;
    }
//    printf("mot cree %d a ligne %d\n", motCree, i);
    if (motCree == 1) {
        if (i != l) {
            while (i > 1 && p.cases[i-1][c-1] != ' ') {
                i--;
            };
        }
//        printf("mot commence a %d %d\n", i, c);
        nm->ligne = i;
        nm->colonne = c;
        nm->direction = 'V';
        strcpy(nm->mot, "");
        do {
            char lettreLue;
            if (i == l) {
                lettreLue = lettre;
            } else {
                lettreLue = p.cases[i-1][c-1];
            }
            strncat(nm->mot, &lettreLue, 1);
            i++;
        } while (i <= TAILLE_PLATEAU && (i == l || p.cases[i-1][c-1] != ' '));
        (*nbMs)++;
    }
}

/**
 * recherche du mot horizontal a partir d une case ou on place une nouvelle lettre
 *
 * @param p le plateau
 * @param l ligne de la case
 * @param c  colonne de la case
 * @param lettre la lettre posee sur la case
 * @param nm pointeur sur le nouveau mot horizontal trouve
 * @param nbMs pointeur sur le nb de nouveaux mots
 */
void rechercherMotHorizontal(Plateau  p, int l, int c, char lettre, Mot* nm, int *nbMs) {

//    printf("recherche mot horizontal %d %d %c\n",l, c, lettre);
    int motCree = 0;
    int i = c;
    if (c > 1 && p.cases[l-1][c-2] != ' ') { // lettre a gauche
        motCree = 1;
        i = c-1;
    }
    if (c < TAILLE_PLATEAU && p.cases[l-1][c] != ' ') { // lettre a droite
        motCree = 1;
    }
//    printf("mot cree %d a colonne %d\n", motCree, i);
    if (motCree == 1) {
        if (i != c) {
            while (i > 1 && p.cases[l-1][i-1] != ' ') {
                i--;
            };
        }
//        printf("mot commence a %d %d\n", l, i);
        nm->ligne = l;
        nm->colonne = i;
        nm->direction = 'H';
        strcpy(nm->mot, "");
        do {
            char lettreLue;
            if (i == c) {
                lettreLue = lettre;
            } else {
                lettreLue = p.cases[l-1][i-1];
            }
            strncat(nm->mot, &lettreLue, 1);
//            printf("mot H %s\n", nm->mot);
            i++;
        } while (i <= TAILLE_PLATEAU && (i == c || p.cases[l-1][i-1] != ' '));
        (*nbMs)++;
    }
}

/**
 * teste si les lettres peuvent etre posees
 *
 * @param c le coup joue par le joueur
 * @param p le plateau
 * @param nbNms pointeur sur le nb de nouveaux mots
 * @param nms pointeur sur les nouveaux mots
 * @param t numero du tour
 * @return 1 si le mot peut etre pose
 * 0 si pas assez de place (en dehors du plateau
 * -1 si hors le centre du plateau au premier tour
 * -2 pas de mot adjacent a partir du 2eme tour
 * -3 mot trop court au premier tour (2 lettres posees minimum)
 */
int testerPoseMot (Coup c, Plateau p, int* nbNms, Mot* nms, int t) {
    int posePossible = 1;
    int poseCentre = 0;
    int motAdjacent = 0;

    *nbNms = 1;

    strcpy(nms[0].mot, "");
    nms[0].direction = c.direction;
    nms[0].ligne = c.ligne;
    nms[0].colonne = c.colonne;

    int motAConstruire = 1;

    int numLettre = 0;
    if (c.direction == 'H') {
        int col = c.colonne;
        do {
            char lettrePlateau = p.cases[c.ligne-1][col-1];
//            printf("lettre plateau = %c\n", lettrePlateau);
            if (lettrePlateau == ' ') {
//            	printf("num / taille = %d / %d\n", numLettre, strlen(c.lettres));
                if (numLettre >= strlen(c.lettres)) { // pas de lettre sur le plateau et plus aucune lettre a placer
//	            	printf ("fin contruction mot\n");
                    motAConstruire = 0;
                } else {
                    rechercherMotVertical(p, c.ligne, col, c.lettres[numLettre], &(nms[*nbNms]), nbNms);
                    strncat(nms[0].mot, &(c.lettres[numLettre]), 1);
                    if (col == 8 && c.ligne == 8) {
                        poseCentre = 1;
                    }
                    motAdjacent += adjacentRempli(p, c.ligne, col);
                    numLettre++;
                }
            } else {
                strncat(nms[0].mot, &lettrePlateau, 1);
            }
//		    printf("mot en construction : %s\n", nms[0].mot);
            col++;
        } while (motAConstruire == 1 && col <= TAILLE_PLATEAU);
    } else if (c.direction == 'V') {
        int lig = c.ligne;
        do {
            char lettrePlateau = p.cases[lig-1][c.colonne-1];
            if (lettrePlateau == ' ') {
                if (numLettre >= strlen(c.lettres)) { // pas de lettre sur le plateau et plus aucune lettre a placer
                    motAConstruire = 0;
                } else {
                    rechercherMotHorizontal(p,lig, c.colonne, c.lettres[numLettre], &(nms[*nbNms]), nbNms);
                    strncat(nms[0].mot, &(c.lettres[numLettre]), 1);
                    if (lig == 8 && c.colonne == 8) {
                        poseCentre = 1;
                    }
                    motAdjacent += adjacentRempli(p, lig, c.colonne);
                    numLettre++;
                }
            } else {
                strncat(nms[0].mot, &lettrePlateau, 1);
            }
            lig++;
        } while (motAConstruire == 1 && lig <= TAILLE_PLATEAU);
    }
    if (numLettre < strlen(c.lettres)) {
        posePossible = 0;
    } else if (t == 0 && poseCentre == 0) {
        posePossible = -1; // pose en dehors du centre du plateau au 1er tour
    } else if (t == 0 && strlen(nms[0].mot) < 2) {
        posePossible = -3; // mot trop court au 1er tour
    } else if (t > 0 && motAdjacent == 0) {
        posePossible = -2; // mot non adjacent avec mots sur plateau
    }
    return posePossible;
}

/**
 * pose les lettres sur le plateau
 *
 * @param c le coup joue
 * @param p pointeur vers le plateau
 */
void poserLettres (Coup c, Plateau *p) {
    if (c.direction == 'H') {
        int col = c.colonne;
        int numLettre = 0;
        do {
            char lettrePlateau = p->cases[c.ligne-1][col-1];
            if (lettrePlateau == ' ') {
                p->cases[c.ligne-1][col-1] = c.lettres[numLettre];
                numLettre++;
            }
            col++;
        } while (numLettre < strlen(c.lettres) && col <= TAILLE_PLATEAU);
    } else if (c.direction == 'V') {
        int lig = c.ligne;
        int numLettre = 0;
        do {
            char lettrePlateau = p->cases[lig-1][c.colonne-1];
            if (lettrePlateau == ' ') {
                p->cases[lig-1][c.colonne-1] = c.lettres[numLettre];
                numLettre++;
            }
            lig++;
        } while (numLettre < strlen(c.lettres) && lig <= TAILLE_PLATEAU);
    }
}

/**
 * affichage du coup
 *
 * @param c le coup a afficher
 */
void voirCoup (Coup c) {
    printf("lettres : %s\n", c.lettres);
    printf("position / direction : %d %d %c\n", c.ligne, c.colonne, c.direction);
}

/**
 * affichage d un nouveau mot
 *
 * @param m
 */
void voirNouveauMot (Mot m) {
    printf("%s a la position %d %d en direction %c\n", m.mot, m.ligne, m.colonne, m.direction);

}

/**
 * cherche la lettre parmi les lettres pour avoir les points associes
 *
 * @param lettre a chercher
 * @param ls pointeurr sur les lettres
 * @return la lettre trouvee
 */
Lettre* chercherLettre (char lettre, Lettre* ls) {//sous programme
    for (int i = 0; i < NB_LETTRES; i++) {
        if (ls[i].lettre == lettre) {
            return &ls[i];
        }
    }
    return NULL;
}

/**
 * calcule les points d un coup (1 ou plusieurs nouveaux mots)
 *
 * @param p le plateau
 * @param nbM nb de nouveaux mots
 * @param ms pointeur sur les nouveaux mots
 * @param ls pointeur sur les lettres
 * @return le nb de points
 */
int compterPoints(Plateau p, int nbM, Mot* ms, Lettre* ls) {
    int points = 0;
    for (int i=0; i < nbM; i++) {
        int coefMot = 1;
        int pointMot = 0;
        if (ms[i].direction == 'H') {
            int c = ms[i].colonne;
            for (int j=0; j < strlen(ms[i].mot); j++) {
                char lettre = ms[i].mot[j];
//                printf("lettre du mot %c\n", lettre);
                Lettre* le = chercherLettre(lettre, ls);
                if (le != NULL) {
//                    voirLettre(*le);
                    if (p.cases[ms[i].ligne-1][c-1] == ' ') { // lettre posee
                        if (p.bonus[ms[i].ligne-1][c-1] == ' ') {
                            pointMot += le->nbPoints;
                        } else {
                            if (p.bonus[ms[i].ligne-1][c-1] == 21) { // mot compte triple
                                pointMot += le->nbPoints;
                                coefMot *= 3;
                            } else if (p.bonus[ms[i].ligne-1][c-1] == '@') { // mot compte double
                                pointMot += le->nbPoints;
                                coefMot *= 2;
                            } else if (p.bonus[ms[i].ligne-1][c-1] == '#') { // mot compte double (centre)
                                pointMot += le->nbPoints;
                                coefMot *= 2;
                            } else if (p.bonus[ms[i].ligne-1][c-1] == '%') { // lettre compte triple
                                pointMot += 3*(le->nbPoints);
                            } else if (p.bonus[ms[i].ligne-1][c-1] == '&') { // lettre compte double
                                pointMot += 2*(le->nbPoints);
                            }
                        }
                    } else {
                        pointMot += le->nbPoints;
                    }
                } else {
                    printf("lettre inconnue : %c\n", lettre);
                    break;
                }
                c++;
            }
        } else if (ms[i].direction == 'V') {
            int l = ms[i].ligne;
            for (int j=0; j < strlen(ms[i].mot); j++) {
                char lettre = ms[i].mot[j];
//                printf("lettre du mot %c\n", lettre);
                Lettre* le = chercherLettre(lettre, ls);
                if (le != NULL) {
//                    voirLettre(*le);
                    if (p.cases[l-1][ms[i].colonne-1] == ' ') { // lettre posee
                        if (p.bonus[l-1][ms[i].colonne-1] == ' ') {
                            pointMot += le->nbPoints;
                        } else {
                            if (p.bonus[l-1][ms[i].colonne-1] == 21) { // mot compte triple
                                pointMot += le->nbPoints;
                                coefMot *= 3;
                            } else if (p.bonus[l-1][ms[i].colonne-1] == '@') { // mot compte double
                                pointMot += le->nbPoints;
                                coefMot *= 2;
                            } else if (p.bonus[l-1][ms[i].colonne-1] == '#') { // mot compte double (centre)
                                pointMot += le->nbPoints;
                                coefMot *= 2;
                            } else if (p.bonus[l-1][ms[i].colonne-1] == '%') { // lettre compte triple
                                pointMot += 3*(le->nbPoints);
                            } else if (p.bonus[l-1][ms[i].colonne-1] == '&') { // lettre compte double
                                pointMot += 2*(le->nbPoints);
                            }
                        }
                    } else {
                        pointMot += le->nbPoints;
                    }
                } else {
                    printf("lettre inconnue : %c\n", lettre);
                    break;
                }
                l++;
            }
        }
        printf("point mot %d coef %d\n", pointMot, coefMot);
        points += pointMot*coefMot;
    }
    return points;
}

/**
 * affiche les points des joueurs
 *
 * @param js pointeur sur les joueurs
 * @param n nb de joueurs
 */
void afficherPointsJoueurs(Joueur* js, int n) {
    for (int i = 0; i < n; ++i) {
        printf("%s : %d points\n", js[i].nom, js[i].score);
    }
    printf("\n\n");
}
/**
 * calcule les points d un chevalet (fin de partie)
 *
 * @param c le chevalet
 * @param ls pointeur sur les lettres
 * @return le nb de points
 */
int calculerPointsChevalet (Chevalet c, Lettre* ls) {
    int nbPointsChevalet = 0;
    for (int i = 0; i < c.nbLettres; ++i) {
        Lettre* le = chercherLettre(c.lettres[i], ls);
        if (le != NULL) {
            nbPointsChevalet += le->nbPoints;
        } else {
            printf("lettre inconnue : %c\n", c.lettres[i]);
            break;
        }
    }
    return nbPointsChevalet;
}

/**
 * joue une partie cree ou reprise
 *
 * @param part pointeur vers la partie
 * @param ls pointeur sur les lettres
 */
void jouerPartie (Partie *part, Lettre* ls) {
    char arret = 'N';
    do {
        printf("**** nouveau tour de la partie : %d\n", part->tour);
        voirJoueur(part->joueurs[part->numJoueur]);
        char choixJeu = ' ';//choix du type de coup
        do {
            printf("veux tu poser un mot (M), passer(P) ou echanger des lettres (E)?\n");
            scanf(" %c", &choixJeu);
            fflush(stdin);
        } while (choixJeu != 'M' && choixJeu != 'P' && choixJeu != 'E');
        if (choixJeu == 'M') {//pour poser un mot
            printf("Lettres a poser\n");
            Coup coup;
            Mot nouveauxMots[TAILLE_PLATEAU];
            int nbNouveauxMots = 0;
            int posePossible = 1;
            do {
                coup = creerCoup();//coup jouer
                voirCoup(coup);
                if (testerLettreChevalet(part->joueurs[part->numJoueur].chevalet, coup.lettres) == 0) {
                    printf("tu n as pas ces lettres dans ton chevalet\n");
                } else {
                    printf("tu as bien ces lettres dans ton chevalet\n");
                }
                posePossible = testerPoseMot(coup, part->plateau, &nbNouveauxMots, nouveauxMots, part->tour);
                if (posePossible == 0) {
                    printf("il n y a pas la place sur le plateau pour poser les lettres\n");
                } else if (posePossible == -1) {
                    printf("le mot ne passe pas par le centre du plateau au 1er tour\n");
                } else if (posePossible == -3) {
                    printf("le mot a moins de 2 lettres au 1er tour\n");
                } else if (posePossible == -2) {
                    printf("le mot n est pas adjacent avec un mot du plateau\n");
                } else {
                    printf("il y a la place sur la plateau pour poser les lettres\n");
                }
            } while (testerLettreChevalet(part->joueurs[part->numJoueur].chevalet, coup.lettres) == 0 || posePossible != 1);

            char motExist;
            do {
                if (nbNouveauxMots == 1) {
                    printf("est ce que le mot existe ?\n");
                } else {
                    printf("est ce que les mots existent ?\n");
                }
                for (int i = 0; i < nbNouveauxMots; ++i) {
                    voirNouveauMot(nouveauxMots[i]);
                }
                scanf(" %c", &motExist);//saisie reponse si les nouveaux mots existent ou non
                fflush(stdin);
            } while (motExist != 'N' && motExist != 'O');

            if (motExist == 'N') {
                printf("tu passes ton tour\n");
            } else if (motExist == 'O') {
                int points = compterPoints(part->plateau, nbNouveauxMots, nouveauxMots, ls);//comptage des points
                if (strlen(coup.lettres) == NB_MAX_LETTRES_CHEVALET) {
                    points += POINTS_SCRABBLE;
                }
                printf("points marques = %d\n", points);
                part->joueurs[part->numJoueur].score += points;
                poserLettres(coup, &(part->plateau));//pose des lettres sur le plateau
                voirChevalet(part->joueurs[part->numJoueur].chevalet);
                retirerLettresChevalet (&(part->joueurs[part->numJoueur].chevalet), coup.lettres);
                voirChevalet(part->joueurs[part->numJoueur].chevalet);//retire les lettres du chevalet
                voirPioche(part->pioche, 0);
                remplirChevalet (&(part->joueurs[part->numJoueur]), &(part->pioche));//pioche les lettres
                voirChevalet(part->joueurs[part->numJoueur].chevalet);
                if (part->joueurs[part->numJoueur].chevalet.nbLettres == 0 && part->pioche.nbJetons == 0) {//fin de partie
                    int pointsTotaux = 0;
                    for (int i = 0; i < part->nbJoueurs; ++i) {
                        if (i != part->numJoueur) {
                            int pointChevalet = calculerPointsChevalet(part->joueurs[i].chevalet, ls);
                            part->joueurs[i].score -= pointChevalet;
                            pointsTotaux += pointChevalet;
                        }
                    }
                    part->joueurs[part->numJoueur].score += pointsTotaux;//point a ajouter au vainqueur
                    printf("%s a termine la partie\n", part->joueurs[part->numJoueur].nom);
                    afficherPointsJoueurs(part->joueurs, part->nbJoueurs);
                    part->fin = 1;
                }
            }
            afficherPlateau(part->plateau, 1);

        } else if (choixJeu == 'P') {//passer son tour
            printf("Tu as passe ton tour\n");
        } else if (choixJeu == 'E') {//echanger des lettres
            char lettresEchangees[NB_MAX_LETTRES_CHEVALET];
            do {
                printf("Lettres a echanger ?\n");
                scanf("%s", lettresEchangees);
                fflush(stdin);

            } while (testerLettreChevalet(part->joueurs[part->numJoueur].chevalet, lettresEchangees) == 0);//verifie si les lettres sont dans le chevalet
            voirChevalet(part->joueurs[part->numJoueur].chevalet);
            retirerLettresChevalet (&(part->joueurs[part->numJoueur].chevalet), lettresEchangees);//retire les lettres du chevalet
            voirChevalet(part->joueurs[part->numJoueur].chevalet);
            voirPioche(part->pioche, 0);
            remplirChevalet (&(part->joueurs[part->numJoueur]), &(part->pioche));//pioche
            voirChevalet(part->joueurs[part->numJoueur].chevalet);
            ajouterJetonsPioche (&(part->pioche), lettresEchangees);//ajoute les jetons a la pioche
            for (int i = 0; i < NB_MELANGE; ++i) {
                melangerPioche(&(part->pioche));//melange la pioche
            }
            voirPioche(part->pioche, 0);
        }

        if (part->fin == 0) {//passage au tour suivant si la partie n est pas finie
            part->tour++;
            part->numJoueur++;
            if (part->numJoueur == part->nbJoueurs) {
                part->numJoueur = 0;
            }
            printf("arret de la partie (O/N) ?\n");//demande un arret de la partie apres chaque tour
            scanf(" %c", &arret);
            fflush(stdin);
        } else {
            arret = 'O';
        }
    } while (arret != 'O');
}

/**
 * gestion du menu
 *
 * @return le choix du menu
 */
int gererMenu () {
    printf("menu scrabble\n\n");
    printf("0 : fin du programme\n");
    printf("1 : nouvelle partie\n");
    printf("2 : enregistrement de la partie en cours\n");
    printf("3 : reprise de la partie enregistree\n");
    printf("4 : affichage des regles du jeu\n");
    printf("5 : affichage des points des joueurs\n\n");
    int choix = 0;
    do {
        printf("tape le numero de ton choix\n");
        scanf("%d", &choix);
        fflush(stdin);
    } while (choix < 0 && choix > 5);
    return choix;
}

/**
 * enregistrement d une partie dans un fichier binaire
 *
 * @param p la partie
 * @return 1 si l enregistrement a bien ete fait et 0 ou -1 si probleme
 */
int enregistrerPartie (Partie p) {
    if (p.nbJoueurs > 0) {
        FILE *fichier;
        fichier = fopen("partie.scr","wb");
        if (fichier != NULL) {
            fwrite(&p,sizeof(p),1,fichier);
            fclose(fichier);
            return 1;
        } else {
            return -1;
        }
    }
    return 0;
}

/**
 * lecture d une partie a partir d un fichier binaire
 *
 * @param p pointeur de la partie
 * @return 1 si lecture a ete faite et 0 sinon
 */
int lirePartie (Partie* p) {
    FILE *fichier;
    fichier = fopen("partie.scr","rb");
    if (fichier != NULL) {
        fread(p,sizeof(*p),1,fichier);
        fclose(fichier);
        return 1;
    }
    return 0;
}
/**
 * programme main
 *
 * @return 0 si fin okay
 */
int main() {
    srand(time(NULL)); // init tirage aleatoire

    Lettre lettres[NB_LETTRES];//creation des lettres
    creerLettres(lettres);
    voirLettres(lettres, NB_LETTRES);

    Partie partie;
    partie.nbJoueurs = 0;

    int choixMenu = 0;

    do {
        choixMenu = gererMenu();
        switch (choixMenu) {
            case 1 :
                printf("Nouvelle partie de scrabble\n\n");
                partie = creerPartie(lettres);//creation partie
                jouerPartie(&partie, lettres);//jouer partie
                break;
            case 2 :
                int enregistrement = enregistrerPartie(partie);//enregistrement de la partie
                if (enregistrement == 1) {
                    printf("La partie a ete enregistree\n\n");
                } else {
                    if (enregistrement == 0) {
                        printf("aucune partie en cours a enregistrer !\n\n");
                    } else if (enregistrement == -1){
                        printf("impossible d ouvrir le fichier en ecriture !\n\n");
                    }
                }
                attenteEspace();
                break;
            case 3 :
                printf("Reprise de la partie enregistree\n\n");
                int lecture = lirePartie(&partie);//lecture de la partie
                if (lecture == 1) {
                    afficherPlateau(partie.plateau, 1);
                    for (int i=0; i<partie.nbJoueurs; i++) {
                        voirJoueur(partie.joueurs[i]);
                    }
                    if (partie.fin == 1) {
                        printf("la partie est finie\n\n");
                    } else {
                        jouerPartie(&partie, lettres);//jouer la partie
                    }
                } else {
                    if (lecture == 0) {
                        printf("impossible d ouvrir le fichier en lecture !\n\n");
                    }
                }
                break;
            case 4 :
                afficherRegles();//affichage des regles
                break;
            case 5 :
                if (partie.nbJoueurs > 0) {
                    afficherPointsJoueurs(partie.joueurs, partie.nbJoueurs);//affichage des scores
                } else {
                    printf("aucune partie en cours\n\n");
                }
                break;
            default:
                break;
        }
    } while (choixMenu != 0);

    printf("fin du programme\n");

}