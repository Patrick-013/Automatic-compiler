#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "Analyse_syntaxique.c"

// examiner si l'automate est déterministe
void is_deterministe()
{	
	//S'il existe seulement une transition, l'automate est déterministe
	if (syntax_res->trans_info_nombre == 1)
		return;

	for (int i = 0; i < syntax_res->trans_info_nombre; i++)
	{
		for (int j = i + 1; j < syntax_res->trans_info_nombre; j++)
		{		
		        // Si les sources et les émissions sont identiques, mais les desitinations ne sont pas les mêmes, l'automate n'est pas déterministe. 
			if ((syntax_res->trans[i]->source == syntax_res->trans[j]->source) && (syntax_res->trans[i]->emission == syntax_res->trans[j]->emission)
			 && (syntax_res->trans[i]->destination != syntax_res->trans[j]->destination))
			{
					printf("\nSémantique erreur : Non deterministe, voir les deux transitions:");
					printf("\ntransition %d : %d -> %d, par %c\n", i + 1, syntax_res->trans[i]->source, syntax_res->trans[i]->destination, syntax_res->trans[i]->emission);
					printf("\ntransition %d : %d -> %d, par %c\n", j + 1, syntax_res->trans[j]->source, syntax_res->trans[j]->destination, syntax_res->trans[j]->emission);
					exit(1);
			}
                       // Si les sources et les émissions et les destinations sont identiques, mais les infos des piles ne sont pas les mêmes, l'automate n'est pas déterministe.
			else if ((syntax_res->trans[i]->source == syntax_res->trans[j]->source) && (syntax_res->trans[i]->emission == syntax_res->trans[j]->emission) 
			&& (syntax_res->trans[i]->destination == syntax_res->trans[j]->destination))
			{
				for (int k = 0; k < 10; k++)
				{
				        // Comparer les infos des piles
					if ((syntax_res->trans[i]->em_ou_de[k] != syntax_res->trans[j]->em_ou_de[k]) || ((syntax_res->trans[i]->pile[k] != syntax_res->trans[j]->pile[k])))
					{
						printf("\nSémantique erreur : Non deterministe, les info des piles pour transitions identiques ne sont pas les mêmes\n");
						exit(1);
						
					}
					
				}
				
			}
		}
	}


}




// examiner le nombre d'états 
void examine_etat_nombre()
{
	for (int i = 0; i < syntax_res->trans_info_nombre; i++)
	{
	        // Examiner les sources
		if ((syntax_res->etat_nombre <= syntax_res->trans[i]->source))
		{
			printf("\nSémantique erreur : le value d'état %d est plus grand que le nombre d'états %d\n", syntax_res->trans[i]->source, syntax_res->etat_nombre);
			exit(1);
		}
               // Examiner les destinations
		else if ((syntax_res->etat_nombre <= syntax_res->trans[i]->destination))
		{	
			printf("\nSémantique erreur : le value d'état %d est plus grand que le nombre d'états %d\n", syntax_res->trans[i]->destination, syntax_res->etat_nombre);
			exit(1);
		}
	}
}

// examiner le nombre des piles
void examine_pile_nombre()
{
	for (int j = 0; j < syntax_res->trans_info_nombre; j++)
	{
		int count = -1;

		for (int k = 0; k < 10; k++)
		{       
		        // noter les actions sur la pile
			if (syntax_res->trans[j]->em_ou_de[k] != 0)
			{   
			    //printf("\n%d", syntax_res->trans[j]->em_ou_de[k]);
			    count++;
			}
	
		}
		//printf("\n%d", count);
		if (count > syntax_res->pile_nombre)
		{
			printf("\nSémantique erreur : le nombre de la pile de transition : %d\n" , count);
			printf("le nombre de pile totals : %d\n", syntax_res->pile_nombre);
			printf("le nombre de la pile de transition > le nombre de pile totals.\n");
			exit(1);
		}
		
	}

}





void analyse_semantique()
{
        is_deterministe();
        //printf("%ld\n", 123123123131);
        
        // examiner le nombre d'état initial
	if (syntax_res->etat_nombre < syntax_res->initial)
	{
		printf("\nSémantique erreur : le nombre d'état initial %d est plus grand que le nombre d'états %d\n", syntax_res->initial, syntax_res->etat_nombre);
		exit(1);
	}
	
	//printf("%ld\n", 123123123131);
	// examiner le nombre d'états finals
	if (syntax_res->etat_nombre <= syntax_res->final_nombre)
	{
		printf("\nSémantique erreur : le nombre d'état final %d est plus grand que le nombre d'états %d\n", syntax_res->final_nombre, syntax_res->etat_nombre);
		exit(1);
	}
	
	//printf("%ld\n", 123123123131);
	
	
	examine_etat_nombre();
	//printf("%ld\n", 123123123131);
	examine_pile_nombre();

	
	printf("\nD'après analyse sémantique, il n'existe pas des erreurs sémantiques");
        printf("\n");
	
}
