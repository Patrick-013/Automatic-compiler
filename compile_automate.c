#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "Analyse_semantique.c"

int vm[100];
int etat_vm_len[20];
int etat_trans_nom[20];
int etat_adresse[20];
int vm_length = 0;

//  Annoncer les fonctions
void vm_etat(int etat);
void vm_trans(int cur_trans);
void compile_automate();
void print_vm_resultat();


void vm_etat(int etat)
{
        // Traversal remplit les informations de chaque état dans le vm
	int index = etat_adresse[etat];
	vm[index] = etat_trans_nom[etat];
	index += 1;

	for (int cur_trans = 0; cur_trans < syntax_res->trans_info_nombre; cur_trans++)
	{
		trans_info* temp = syntax_res->trans[cur_trans];
		// Stocker en continu toutes les transitions de chaque état dans vm
		if (etat == temp->source)
		{
			vm[index] = temp->emission;
			index += 1;

			vm[index] = etat_adresse[temp->destination];
			index += 1;

			for (int i = 0; i < syntax_res->pile_nombre; i++)
			{
				vm[index] = temp->pile[i];
				index += 1;
				vm[index] = temp->em_ou_de[i];
				index += 1;

			}
		}
	}

}

void vm_trans(int cur_trans)
{


	int source = syntax_res->trans[cur_trans]->source;
	// Mettre à jour la longueur de chaque état (2 est longueur de source et destination)
	etat_vm_len[source] += 2 * syntax_res->pile_nombre + 2;
	// Mettre à jour le nombre de transitions pour chaque état
	etat_trans_nom[source] += 1;
}

void compile_automate()
{
	int etat_nombre = syntax_res->etat_nombre;
	int initial_etat = syntax_res->initial;
	int trans_info_nombre = syntax_res->trans_info_nombre;
	int final_nombre = syntax_res->final_nombre;

	// noter les informations des transitions pour réserver
	for (int cur_trans = 0; cur_trans < trans_info_nombre; cur_trans++) {
		vm_trans(cur_trans);
	}

	// calculer l'adresse de chaque état
	int cur_index = final_nombre + 3;
	for (int etat = 0; etat < etat_nombre; etat++)
	{
		etat_adresse[etat] = cur_index;
		cur_index += (1 + etat_vm_len[etat]);
	}
        
        // noter le longueur de vm
	vm_length += cur_index;

	
	// Le type des trois premiers élements sont sûrs. 

	vm[0] = syntax_res->pile_nombre;
	vm[1] = etat_adresse[initial_etat];
	vm[2] = syntax_res->final_nombre;

        // réserver les finals adresses
	for (int i = 0; i < final_nombre; i++)
		vm[3 + i] = etat_adresse[syntax_res->final[i]];
		
        // Remplisser les informations de chaque état dans la machine virtuelle
	for (int etat = 0; etat < etat_nombre; etat++)
		vm_etat(etat);
        
        // stocker la table des symboles dans TS.txt
	FILE* ts = fopen("TS.txt", "w");
	for (int i = 0; i < syntax_res->etat_nombre; i++)
		fprintf(ts, "le Nom d'état = %s, l'adresse = %d\n", syntax_res->etats[i], etat_adresse[i]);
	fclose(ts);
	
        // Stocker vm
	FILE* vm_file = fopen("VM", "wb");
	fwrite(vm, sizeof(int), vm_length, vm_file);
	fclose(vm_file);
        
	
	print_vm_resultat();
}

void print_vm_resultat()
{
	printf("\nTableau VM:\n");
	for (int i = 0; i < vm_length; i++) {
		printf("%d ", vm[i]);
	}
	printf("\n");
	printf("\nle nombre de pile = %d", vm[0]);
	printf("\nl'adresse initiale = %d", vm[1]);
	printf("\nle nombre des états finals = %d", vm[2]);
	printf("\nl'adresse des états finals = ");

	for (int i = 0; i < vm[2]; i++) {
		printf("%d  ", vm[i + 3]);
	}

	int cur_adresse = vm[2] + 3;
	printf("\n");
        
        // Imprimer les informations pour chaque état à tour de rôle
	for (int i = 0; i < syntax_res->etat_nombre; i++)
	{
		printf("\nétat %d:", i);
		printf("\nle nom d'état = %s, l'adresse de source = %d  ", syntax_res->etats[i], etat_adresse[i]);
		int trans_nombre = vm[cur_adresse];
		printf("\nle nombre de transition = %d  ", trans_nombre);
		cur_adresse += 1;
                
                // Les informations de la pile
		for (int _ = 0; _ < trans_nombre; _++)
		{
			printf("\ndéclencher %c, l'adresse de destination = %d  ", vm[cur_adresse], vm[cur_adresse + 1]);
			cur_adresse += 2;

			for (int j = 0; j < vm[0]; j++)
			{
				if (vm[cur_adresse + 1] == 1)
					printf("Pile %d : empiler %c   ", j + 1, vm[cur_adresse]);
				else if (vm[cur_adresse + 1] == -1)
					printf("Pile %d : dépiler %c   ", j + 1, vm[cur_adresse]);
				else
				{
				}

				cur_adresse += 2;
			}
		}
		printf("\n");
	}
	printf("\n");
}


