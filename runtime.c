#include <stdbool.h>
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_NOMBRE_ADRESSE 100
#define MAX_NOMBRE_FINAL 20

int vm[MAX_NOMBRE_ADRESSE];
int vm_length;

int etat_length;
int pile_nombre;
int initial_etat_adresse;
int final_nombre;
int final_etat_adresse[MAX_NOMBRE_FINAL];
char input_sequence[20];

bool is_Vide();
void analyse_vm();
void analyse_ts();
void runtime(char* s, int len);
int prochaine_trans(int trans_index, char cur_char);

// Définir la structure état pour stocker les informations extraites des fichiers VM et TS.txt
typedef struct etat_info
{
	int etat_adresse[10];
	char etat_nom[20][20];
} etat_info;

// Definir la nouvelle pile pour realiser les operations sur different adresses des elements des piles 
typedef struct pile
{
	void (*push)(int*, char*, char);
	char (*pop)(int*, char*);
	int (*vide)(int);
	int hauteur;
	char data[100];
} pile;


pile** mapile;
etat_info* maetat;


int debug = 1;

void push_aux(int* addr, char* data, char mot)
{
	int hauteur = *addr;
	if (hauteur == 100)
	{
		printf("Pile pleine !\n");
		exit(2);
	}
	else {
		*addr = hauteur + 1;
		data[hauteur] = mot;
	}
};

char pop_aux(int* addr, char* data)
{
	int hauteur = *addr;
	if (hauteur == 0)
	{
		return -1;
	}
	else {
		*addr = hauteur - 1;
		return(data[hauteur - 1]);
	}
};


int vide_aux(int hauteur)
{
	return(hauteur == 0);
};

// juger si les piles sont vides
bool is_Vide()
{
	int count = 0;
	for (int i = 0; i < pile_nombre; ++i)
	{
		if (mapile[i]->hauteur != 0)
		{
			count++;
		}
	}

	if (count == 0)
	{
		return true;
	}

	else {

		return false;
		printf("\n");
	}
}




int prochaine_trans(int trans_index, char cur_char)
{

	//le nombre de transitions d'etat
	int trans_nombre = vm[trans_index];
	// le longueur de transition

	int trans_len = 2 * pile_nombre;

	trans_len += 2;

	// initialisation de index actuelle a plaisir
	int cur_index = 521;

	trans_index += 1;

	for (int i = 0; i < trans_nombre; i++)
	{
		// Si le mot d'entree est correspondant avec vm
		if (cur_char == (char)vm[trans_index + trans_len * i])
		{
			// mis a jour l'index
			cur_index = vm[trans_index + 1 + trans_len * i];

			for (int j = 0; j < pile_nombre; j++)
			{
				int pile_len = 2;
				int ecart = 3;
				int pile_value = trans_index + ecart + pile_len * j + trans_len * i;
				// Si pile_value corresspond "push"
				if (vm[pile_value] == 1)
					mapile[j]->push(&(mapile[j]->hauteur), mapile[j]->data,
						(char)vm[pile_value - 1]);

				// Si pile_value corresspond "pop"
				else if (vm[pile_value] == -1)
				{

					int  p_mot = mapile[j]->pop(&(mapile[j]->hauteur), mapile[j]->data);
					// Si pas de mots dans la pile, erreur!
					if (p_mot == -1)
					{
						if (debug == 1) {
							printf("%c -> Erreur : Pile %d vide !\n", cur_char, j + 1);
						}
						printf("Le mot %s est refusé ! \n", input_sequence);
						exit(0);
					}

				}

			}

			// Remarque : La boucle autour de i n'est effectuée qu'une seule fois et revient à la fonction d'exécution après la fin
			break;
		}
		// Sinon , rentrez dans la boucle, vérifiez toutes les transitions à partir de l'état actuel et voyez s'il y a une correspondance avec le mot d'entrée.

	}

	// Si les transitions de tous les états actuels ne correspondent pas, le résultat d'entrée est rejeté
	if (cur_index == 521)
	{
		if (debug)
		{
			printf("%c -> Erreur : Aucune des transitions ne correspond au caractère actuel %c !\n", cur_char, cur_char);
		}

		printf("Le mot %s est refusé \n", input_sequence);
		exit(0);
	}

	return cur_index;

}

void analyse_vm()
{
	FILE* vm_fichier = fopen("VM", "r");

	vm_length = 0;
	size_t len = 10;
	while (len == 10) {
		len = fread(vm + vm_length, sizeof(int), 10, vm_fichier);
		vm_length += len;
	}
	fclose(vm_fichier);

	pile_nombre = vm[0];
	initial_etat_adresse = vm[1];

	final_nombre = vm[2];

	for (int i = 0; i < final_nombre; i++)
	{
		final_etat_adresse[i] = vm[i + 3];
	}

}


void analyse_ts()
{
	FILE* ts_fichier = fopen("TS.txt", "r");

	if (ts_fichier == NULL)
	{
		printf("le fichier n'existe pas!\n ");
		exit(-1);
	}

	etat_length = 0;
	// lire le texte
	while (!feof(ts_fichier))
	{      // "le Nom d'état = %s, l'adresse = %d\n"
		   //"Nom = \"%s\", addresse = %d\n"
		fscanf(ts_fichier, "le Nom d'état = \"%[^\"]\", l'adresse = %d\n", maetat->etat_nom[etat_length], maetat->etat_adresse + etat_length);
		etat_length += 1;
	}
}

void runtime(char* s, int len)
{
	int cur_adresse = initial_etat_adresse;
	char* cur_nom;

	// debug mode
	if (debug)
	{
		// extraire le nom d'etat
		for (int i = 0; i < etat_length; i++)
		{
			if (maetat->etat_adresse[i] == cur_adresse)
			{
				cur_nom = maetat->etat_nom[i];
			}
		}

		printf("  -> État : %s  ", cur_nom);

		// imprinter les informations des piles
		for (int i = 0; i < pile_nombre; i++)
		{
			printf("Pile %d : ", i + 1);
			if (mapile[i]->hauteur == 0)
			{
				printf("Vide   ");
			}
			else
			{
				for (int j = 0; j < mapile[i]->hauteur; j++)
				{
					printf("%c", mapile[i]->data[j]);
				}

				// aligner chaque ligne
				for (int k = 0; k < 7 - mapile[i]->hauteur; k++)
				{
					printf(" ");
				}
			}
		}

		printf("\n");

		for (int i = 0; i < len; i++)
		{
			// determiner l'adresse de chaque mot dans input sequence.
			cur_adresse = prochaine_trans(cur_adresse, s[i]);

			// imprinter les informations des piles
			for (int i = 0; i < etat_length; i++)
			{
				if (maetat->etat_adresse[i] == cur_adresse)
				{
					cur_nom = maetat->etat_nom[i];
				}
			}

			printf("%c -> État : %s  ", s[i], cur_nom);

			for (int j = 0; j < pile_nombre; j++)
			{
				printf("Pile %d : ", j + 1);
				if (mapile[j]->hauteur == 0)
				{
					printf("Vide   ");
				}

				else
				{
					for (int k = 0; k < mapile[j]->hauteur; k++)
					{
						printf("%c", mapile[j]->data[k]);
					}
					// aligner chaque ligne
					for (int k = 0; k < 7 - mapile[j]->hauteur; k++)
					{
						printf(" ");
					}
				}

			}

			printf("\n");
		}
	}
	// Non debug mode
	else {
		for (int i = 0; i < len; ++i)
			cur_adresse = prochaine_trans(cur_adresse, s[i]);
	}

	// Si la pile est vide finalement, le mot est accepté!
	if (is_Vide() == true)
		printf("Le mot %s est accepté ! \n", s);
	else {
		printf("Le mot %s est refusé ! ", s);
		printf("Piles non vides \n");
	}
}


int main(int argc, char** argv)
{
	// input erreur
	if (argc != 2 && argc != 3) {
		printf("Erreur dans le paramètre d'entrée!\n");
		exit(2);
	}
	if (argc == 2)
	{
		// non debug mode
		debug = 0;
		if (strcmp(argv[1], "VM") != 0) {
			printf("Erreur dans le paramètre d'entrée!\n");
			exit(2);
		}


	}

	else if (argc == 3)
	{
		// debug mode
		debug = 1;
		if (strcmp(argv[1], "-debug") != 0 || strcmp(argv[2], "VM") != 0) {
			printf("Erreur dans le paramètre d'entrée!\n");
			exit(2);
		}


	}

	// lire VM texte
	analyse_vm();

	// initialisation de pile
	mapile = malloc(sizeof(int) * pile_nombre);
	for (int i = 0; i < pile_nombre; i++)
	{
		mapile[i] = malloc(sizeof(pile));

		for (size_t k = 0; k < 100; k++)
			mapile[i]->data[k] = 0;

		mapile[i]->hauteur = 0;
		mapile[i]->push = push_aux;
		mapile[i]->pop = pop_aux;
		mapile[i]->vide = vide_aux;
	}


	// initialisation d'etat
	maetat = malloc(sizeof(etat_info));

	// lire TS.txt
	analyse_ts();

	// input the sequence
	printf("Donner le mot d'entrée: ");
	scanf("%30[^\n]", input_sequence);

	//Execution
	runtime(input_sequence, strlen(input_sequence));
}
