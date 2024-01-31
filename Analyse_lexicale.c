#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#define MAX_BUFFER 2048 
#define MAX_LENGTH 20

//utilser enum pour numeroser les different types
typedef enum lexeme_t
{
    Mot_clef, // Il comtient 5 mots: Automate etats initial final transitions
    Symbole, // [=()[]{}]+ Tous les symboles sauf les guillemets simples, les guillemets doubles et les symboles de commentaire.
    Entier, // [Entiers pas entre guillemets
    Chaine, // les caractères simples entre guillemets simples
    Caractere, // les caractères entre guillemets
    Fleche // →  
} lexeme_t;

//structure de type et expression de chaque lexeme
typedef struct lexeme
{
    lexeme_t type;
    char * expression;
}lexeme;

//structure d'adress et contenu(type et expression) de chaque member 
typedef struct indice
{
    lexeme* content;
    int* address;
    // int* tail;
}indice;

//structure d'une liste des tous les expressions
typedef struct mot_liste
{
    int length;
    char * liste;
}mot_liste;



regex_t myregex_mot_clef;
regex_t myregex_symbole;
regex_t myregex_entier;
regex_t myregex_chaine;
regex_t myregex_caractere;
regex_t myregex_fleche;

int num; //Le nombre de lexemes dans le texte

//les expressions régulières
char* mode_mot_clef = "(Automate|etats|initial|final|transition)";
char* mode_entier = "[0-9]+";                                                     
char* mode_chaine = "\"[^\".]+\"";
char* mode_caractere = "`[^`.]+`";
char* mode_fleche = "[→]";

// Examiner les commentaires et puis les enlever
mot_liste* examine_commentaire(FILE *fp)
{   
    //Définition de deux pointeurs
    int count = 0;
    int char_cur;
    int char_tmp = fgetc(fp);

    // création de liste sans commentaire
    mot_liste* liste_sans_commentaire = malloc(sizeof(mot_liste));
    liste_sans_commentaire->length = MAX_BUFFER;
    liste_sans_commentaire->liste = malloc(sizeof(char)*MAX_BUFFER);

    while ((char_cur = fgetc(fp)) && char_cur!= -1){
        // Tout d'abord nous négligeons les commentaires sur une ligne(par '//')
        if (char_tmp == '/'){   
            if (char_cur == '/'){
                while ((char_tmp = fgetc(fp)) &&  char_tmp!= -1){
                    if (char_tmp == '\n') break;
                }
                // A la fin du fichier
                if (char_tmp == EOF) {
                    liste_sans_commentaire->liste[count++] = '\n';
                    break;
                }
            }
            
            // Ensuite nous négligeons les commentaires sur plusieurs lignes(par '/*...*/')
            else if (char_cur == '*'){
                while ((char_cur = fgetc(fp)) && char_cur != -1){
                    if (char_cur =='/' && char_tmp == '*'){
                        char_tmp = '\n';
                        break;
                    }
                    //mis à jour char_tmp
                    else {
                        char_tmp = char_cur;
                    }
                }
            }
            else{
                 liste_sans_commentaire->liste[count++] = (char)char_tmp;
                 //mis à jour char_tmp
                 char_tmp = char_cur;
            }
        }
        // Pas commentaires
        else{
                 liste_sans_commentaire->liste[count++] = (char)char_tmp;
                 //mis à jour char_tmp
                 char_tmp = char_cur;
            }
    }
    
        
    liste_sans_commentaire->liste[count++] = (char)char_tmp;
    liste_sans_commentaire->length = count;
    
    
    return liste_sans_commentaire;
}

void estSymbole(mot_liste* motliste, indice* member){
    
    char symbole;
    lexeme* lex;
    char* expression = malloc(sizeof(char) * 2);
    for (int i = 0; i < motliste->length; i++){
        symbole = motliste->liste[i];
        if (symbole == '=' || symbole == ','){
            
            lex = malloc(sizeof(lexeme));
            expression = malloc(sizeof(char) * 2);
            //chaque valeur contient un '\0'
            expression[0] = motliste->liste[i];
            expression[1] = '\0';
            
            lex->expression = expression;
            lex->type = Symbole;

            member->address[num] = i;
            member->content[num].expression = malloc(sizeof(char) * 2);
            member->content[num].type = Symbole;
            member->content[num].expression = expression;
            num++;
        }
        
        else if (symbole == '(' || symbole == ')' || symbole == '[' || symbole == ']' || symbole == '{' || symbole == '}'){
            
            
            expression = malloc(sizeof(char) * 2);
            //chaque valeur contient un '\0'
            expression[0] = motliste->liste[i];
            expression[1] = '\0';
            

            member->address[num] = i;
            member->content[num].expression = malloc(sizeof(char) * 2);
            member->content[num].type = Symbole;
            member->content[num].expression = expression;
            num++;
        }
        else continue;
    }
}

void regular_matching(mot_liste* motliste, regex_t regexpr, lexeme_t type, indice* member) {
    
    int debut;
    int fin;
    int dernier_indice = 0;
    int res = 0;
    regmatch_t pmatch[1];
    char* p = motliste->liste;

    while(1){
        // nous exécutons l'expression régulière compilé
        res = regexec(&regexpr, p, 1, pmatch, 0); // pmatch est un tableau contenant les résultats
        
        if (res != 0) break; // nous arrêtons quand n'a rien filtré
        
        //début et fin sont les indices du texte ou est l'expression reconnue, à partir de l'indice p
        debut = pmatch[0].rm_so; // le début du motif filtré
        fin = pmatch[0].rm_eo; // fin juste après le motif
        
        //si le type est Fleche, nous devons compter deux mots supplémentaires.
        if (type == Fleche)
        {
            fin += 2;
        }
        
        //si nous trouve un entier dans guillemets simples ou doubles, nous ne le gardons pas comme un entier et mis à jour p et 
        if (type == Entier && debut + dernier_indice != 0 && motliste->liste[debut + dernier_indice - 1] == '`' || motliste->liste[debut + dernier_indice - 1] == '"' ){
            p += fin;
            dernier_indice += fin;
            continue;
        }
        
        char* expression = malloc(sizeof(char)* (fin - debut + 1));
        
        for (int i = 0; i< fin - debut; i++){
            expression[i] = motliste->liste[debut + dernier_indice + i];
        }
        
        expression[fin - debut] = '\0';
        
        
        //Marquer l'addresse et la valeur correspondante de chaque lexème apparié 
        member->content[num].expression = malloc(sizeof(char) * MAX_LENGTH);
        member->address[num] = debut + dernier_indice;
        member->content[num].expression = expression;
        member->content[num].type = type;
        num++;
        
        //Lorsqu'un lexème est apparié, nous mettons à jour les valeurs du pointeur p et de l'offset du point final du précédent, et redémarrons pour faire correspondre le lexème suivant
        p += fin;
        dernier_indice += fin;
    }
}

// Nous construire une fonction pour arranger les lexemes par ordre donné dans le texte.
lexeme* arranger_lexeme(int* array, int len, lexeme* liste_lexeme){
    
    int i;
    int j;
    int temp;
    lexeme temp_lexeme;
 
    for (i = 0 ; i < len - 1 ; i++) 
    {
        int min = i;                  
        
        for (j = i + 1; j < len; j++)     
        {
            if (array[j] < array[min])    
            {
                min = j;    
            }
        }
        if(min != i)
        {
            temp = array[min];  
            temp_lexeme = liste_lexeme[min];
            array[min] = array[i];
            liste_lexeme[min].type = liste_lexeme[i].type;
            liste_lexeme[min].expression = liste_lexeme[i].expression;
            array[i] = temp;
            liste_lexeme[i].type = temp_lexeme.type;
            liste_lexeme[i].expression = temp_lexeme.expression;
            
        }
    }
    return liste_lexeme;
}


lexeme* analyse_lexicale(char *fichier)
{
    num = 0;
    // lire le fichier puis le convertir dans la version sans commentaires
    FILE *fp = fopen(fichier, "r");
    if (fp == NULL){
		fprintf(stderr, "Impossible d'ouvrir le ficher\n");
		exit(1);
	}
    mot_liste* liste_sans_commentaire = examine_commentaire(fp);
    
    /*
    for (int i = 0; i < liste_sans_commentaire->length; i++) 
    { 
    	putchar(liste_sans_commentaire->liste[i]); 
    }
    */
    
    // Allouer de l'espace mémoire pour les variables
    indice* member = malloc(sizeof(indice));
    member->address = malloc(sizeof(int) * MAX_BUFFER);
    member->content = malloc(sizeof(lexeme) * MAX_BUFFER);
    
    // Faire correspondre le lexème par type
    regcomp(&myregex_fleche, mode_fleche, REG_EXTENDED);
    regular_matching(liste_sans_commentaire, myregex_fleche, Fleche, member);
    
    regcomp(&myregex_mot_clef, mode_mot_clef, REG_EXTENDED);
    regular_matching(liste_sans_commentaire, myregex_mot_clef, Mot_clef, member);
    
    regcomp(&myregex_entier, mode_entier, REG_EXTENDED);
    regular_matching(liste_sans_commentaire, myregex_entier, Entier, member);
    
    regcomp(&myregex_chaine, mode_chaine, REG_EXTENDED);
    regular_matching(liste_sans_commentaire, myregex_chaine, Chaine, member);
    
    regcomp(&myregex_caractere, mode_caractere, REG_EXTENDED);
    regular_matching(liste_sans_commentaire, myregex_caractere, Caractere, member);
    
    estSymbole(liste_sans_commentaire, member);

    lexeme* res = malloc(sizeof(lexeme) * MAX_BUFFER);
    res = arranger_lexeme(member->address, num, member->content);

    //Enfin nous libérons les espaces
    free(liste_sans_commentaire->liste);
    free(liste_sans_commentaire);
    free(member->address);
    free(member->content);
    free(member);

    return res;
}





