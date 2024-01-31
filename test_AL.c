#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "Analyse_lexicale.c"

int main(){
    printf("Ficher Nom :  \n");

    char ficher_Nom[20];
    scanf("%s", ficher_Nom);
    
    lexeme* res = analyse_lexicale(ficher_Nom);
    
    printf("Il n'exist pas des errors!\n");
    
    for (int i = 0; res[i].expression; i++) 
    {    printf("\n%d: type: " , i + 1);

	 	switch (res[i].type)
	 	{
	 		case 0: printf("Mot_clef"); break;
	 		case 1: printf("Symbole"); break;
	 		case 2: printf("Entier"); break;
	 		case 3: printf("Chaine"); break;
	 		case 4: printf("Caractere"); break;
	 		case 5: printf("Flech"); break;
	 		default:printf("NON Trouve !");
	 	}
         printf(", l'expression: %s", res[i].expression);
    }
    printf("\n");
}
