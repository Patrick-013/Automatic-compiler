#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analyse_syntaxique.c"

syntax_info* syntax_res;
lexeme* res;
int lexeme_total_length = 0;
int lexeme_address = 0;

int main() {


	printf("Ficher Nom : \n");

	//char ficher_Nom[20];
	//scanf("%s", ficher_Nom);

	lexeme* res = analyse_lexicale("Dpile.txt");

	printf("Il n'exist pas des errors!\n");

	for (int i = 0; res[i].expression; i++)
	{
		lexeme_total_length++;
		printf("\n%d: type: ", i + 1);

		if (res[lexeme_address].type == 0){
	            printf("Mot_clef");
	        }
                else if (res[lexeme_address].type == 1){
                    printf("Symbole");
                }  
                else if (res[lexeme_address].type == 2){
                    printf("Entier");
                }
                else if (res[lexeme_address].type == 3){
                    printf("Chaine");
                }
                else if (res[lexeme_address].type == 4){
                    printf("Caractere");
                }
                else if (res[lexeme_address].type == 5){
                    printf("Flech");
                }
                else printf("Rien!");
		printf(", l'expression: %s", res[i].expression);
	}
        
	//printf("%d", lexeme_total_length);
	//printf("\n%s\n", res[6].expression);
	//lexeme_address += 12232323232;
	syntax_res = malloc(sizeof(syntax_info));
	analyse_syntaxique(res);

	print_resultat();
	/*for (int i = 0; i < 10; i++){
		printf("%d", syntax_res->trans[3]->em_ou_de[i]);
	}
	*/
}

