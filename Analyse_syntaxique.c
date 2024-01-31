#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "Analyse_lexicale.c"
#include <stdbool.h>


int hauteur = 0;
char data[50];


extern lexeme* res;
extern int lexeme_total_length;
extern int lexeme_address;

void print_resultat();
void is_error(lexeme* res);
bool is_egale(bool flag, lexeme* res);
bool is_virgule(bool flag, lexeme* res);
int is_entier(lexeme* res);
char is_caractere(lexeme* res);
void is_parenthese(char symbole, lexeme* res);
void analyse_motclef(lexeme* res);
void analyse_motclef_automate(lexeme* res);
void analyse_motclef_etats(lexeme* res);
void analyse_motclef_initial(lexeme* res);
void analyse_motclef_final(lexeme* res);
void analyse_motclef_trans(lexeme* res);
void analyse_syntaxique(lexeme* res);


typedef struct pile
{
	char (*pop)(void);
	void (*push)(char);
	int (*vide)(void);
} pile;

pile Mapile;

typedef struct trans_info
{
	int source; // la source de lexeme dans une trasition
	int destination; // la destination de lexeme dans une transition
	char emission; // l'emission du lexeme 
	int em_ou_de[10]; // empiler:1 depiler: -1
	char pile[10]; // la pile contient des lexemes qui empilent

} trans_info;

typedef struct syntax_info
{
	char etats[20][20];
	int etat_nombre; // l'automate avoir plusieurs etats
	int initial; // il existe seulement un etat initial
	int final[15]; // l'automate peut avoir plusieurs etats final 
	int final_nombre;
	int trans_info_nombre; // le nomber de transitions 
	int pile_nombre; // nombre de pile
	trans_info* trans[50];
} syntax_info;

extern syntax_info* syntax_res;


char pop_aux(void)
{
	if (hauteur == 0)
	{
		printf("Pile vide !\n");
		exit(1);
	}
	else
		return(data[--hauteur]);
};

void push_aux(char c)
{
	if (hauteur == 50)
	{
		printf("Pile pleine !\n");
		exit(2);
	}
	else
		data[hauteur++] = c;
};



int vide_aux(void)
{
	return(hauteur == 0);
};



// Si il existe des syntaxique erreurs dans resultat lexicale
void is_error(lexeme* res)
{
	printf("\nSyntaxique erreur est trouvé pour '%s', address est %d, type est ", res[lexeme_address].expression, lexeme_address);
        
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
        
	printf("\n");
	exit(3);
};


// Si le symbole est "="
bool is_egale(bool flag, lexeme* res)
{

	if ((res[lexeme_address].type != Symbole) | (res[lexeme_address].expression[0] != '='))
	{
		if (flag)
			is_error(res);
		else
			return false;
	}
	return true;
}

// Si le symbole est ","
bool is_virgule(bool flag, lexeme* res)
{

	if ((res[lexeme_address].type != Symbole) | (res[lexeme_address].expression[0] != ','))
	{
		if (flag)
			is_error(res);
		else
			return false;
	}
	return true;
}

// Si c'est un entier
int is_entier(lexeme* res)
{

	if (res[lexeme_address].type != Entier)
		is_error(res);

	return atoi(res[lexeme_address].expression);
}

// Si c'est un caractere
char is_caractere(lexeme* res)
{


	if (res[lexeme_address].type != Caractere)
		is_error(res);

	return res[lexeme_address].expression[1];
	
}

// Juger si les parenthese sont bien adaptés
void is_parenthese(char symbole, lexeme* res)
{
	//printf("%c", res[lexeme_address].expression[0]);
	if (res[lexeme_address].type != Symbole)
		is_error(res);

	char current_symbole = res[lexeme_address].expression[0];

	// si le symbole sur lexemeRes[lexeme_address] n'est pas ce qu'on souhaite.
	if (symbole != '\0')
		if (current_symbole != symbole)
			is_error(res);
	if (current_symbole == '(') {
		Mapile.push('(');
	}
	else if (current_symbole == '[') {
		Mapile.push('[');
	}
	else if (current_symbole == '{') {
		Mapile.push('{');
	}
	else if (current_symbole == ')') {
		if (Mapile.vide()) {
			is_error(res);
		}
		else if (Mapile.pop() == '(') {
			return;
		}
		else {
			is_error(res);
		}

	}
	else if (current_symbole == ']') {
		if (Mapile.vide()) {
			is_error(res);
		}
		else if (Mapile.pop() == '[') {
			return;
		}
		else {
			is_error(res);
		}

	}
	else if (current_symbole == '}') {
		if (Mapile.vide()) {
			is_error(res);
		}
		else if (Mapile.pop() == '{') {
			return;
		}
		else {
			is_error(res);
		}

	}
	else is_error(res);


}

void search_pile_info(trans_info* current_trans, lexeme* res)
{
	// le nombre des actions sur la pile
	int action_nombre = 0;

	while (1)
	{

		is_parenthese('(', res);
		lexeme_address += 1;

		// le cas pour "()"
		if (res[lexeme_address].expression[0] == ')')
		{
			action_nombre ++;
			current_trans->em_ou_de[0] = 0;
		}

		else {

			if ((lexeme_address + 1 >= lexeme_total_length) | (res[lexeme_address + 1].type != Symbole)
				| (res[lexeme_address + 1].expression[0] != ','))
			{
				lexeme_address += 1;
				is_error(res);
			}


			// le cas pour (→, 'a')
			if (res[lexeme_address].type == Fleche & (strcmp(res[lexeme_address].expression, "→") == 0))
			{
				lexeme_address += 2;
				if (res[lexeme_address].type == Caractere)
				{
					current_trans->em_ou_de[action_nombre] = 1;
					current_trans->pile[action_nombre] = res[lexeme_address].expression[1];
					lexeme_address += 1;
					action_nombre ++;
				}
			}
			
			// le cas pour ('a', →)
			else if (res[lexeme_address].type == Caractere)
			{
				lexeme_address += 2;
				if ((res[lexeme_address].type == Fleche) & (strcmp(res[lexeme_address].expression, "→") == 0))
				{
					current_trans->em_ou_de[action_nombre] = -1;
					current_trans->pile[action_nombre] = res[lexeme_address - 2].expression[1];
					lexeme_address += 1;
					action_nombre ++;
				}
			}

			else {
				is_error(res);
			}
		}

		is_parenthese(')', res);

		lexeme_address += 1;
		if (res[lexeme_address].type == Symbole)
		{
			if (res[lexeme_address].expression[0] == ',')
			{
			}
			else if (res[lexeme_address].expression[0] == ')')
				break;
			else {
				is_error(res);
			}
		}
		else {
			is_error(res);
		}

		lexeme_address += 1;
	}

	is_parenthese(')', res);

}

// analyser les cinq différents mots clefs
void analyse_motclef(lexeme* res)
{

	//printf("\n%ld\n", 12232323232);

	char* mot_clef = res[lexeme_address].expression;
	//printf("%d", strcmp(motclef, "Automate"));
        
       
	if (!strcmp(mot_clef, "Automate")) {
		analyse_motclef_automate(res);
		//printf("%d", lexeme_address);
	}

	else if (!strcmp(mot_clef, "etats")) {
		analyse_motclef_etats(res);
	}

	else if (!strcmp(mot_clef, "initial")) {
		analyse_motclef_initial(res);
		//printf("%d", lexeme_address);
	}

	else if (!strcmp(mot_clef, "final")) {
		analyse_motclef_final(res);
		//printf("\n%d\n", lexeme_address);
	}

	else if (!strcmp(mot_clef, "transition")) {
		//printf("\n%d\n", lexeme_address);
		analyse_motclef_trans(res);
	}
	else
		is_error(res);
		
}

// analyser mot clef "Automate"
void analyse_motclef_automate(lexeme* res)
{
	// la forme : Automate(pile_nombre)
	
	// empiler '('
	lexeme_address += 1;
	is_parenthese('(', res);
	//printf("%d", lexeme_address);

	// noter le nombre de pile
	lexeme_address += 1;
	syntax_res->pile_nombre = is_entier(res);
	//printf("%d", syntax_res->pile_nombre);
	//printf("%d", lexeme_address);

	// depiler '('
	lexeme_address += 1;
	is_parenthese(')', res);
	//printf("%d", lexeme_address);

	// examine '='
	lexeme_address += 1;
	is_egale(1, res);
	//printf("%d", lexeme_address);

        // empiler '{'
	lexeme_address += 1;
	is_parenthese('{', res);
	//printf("%d", lexeme_address);

	lexeme_address += 1;
	//printf("%d", lexeme_address);
}

// analyser mot clef "etats"
void analyse_motclef_etats(lexeme* res)
{
	// la forme: etats = [`char`] ou ["chaine"] 
	// examine '='
	lexeme_address += 1;
	is_egale(1, res);

	// empiler [ 
	lexeme_address += 1;
	is_parenthese('[', res);

	int cnt = 0;
	int etats_num = 0;

	lexeme_t type = res[lexeme_address + 1].type;

	if ((type != Caractere) & (type != Chaine))
	{
		printf("\nN'est pas caractere ou chaine dans les etats");
		is_error(res);
	}

	while (res[lexeme_address + 1].type == type) {
		// [`char`] ou ["chaine"] 
		lexeme_address += 1;
		//noter l'expression des etats
		strncpy(syntax_res->etats[etats_num], res[lexeme_address].expression, 30);
		etats_num++;
		// si ',' , on continue; sinon, break
		lexeme_address += 1;
		if (res[lexeme_address].type != Symbole)
			is_error(res);
		if (res[lexeme_address].expression[0] != ',')
			break;
		cnt++;

		// 
		if (cnt > 8)
			break;
	}

	// depiler ']'
	is_parenthese(']', res);
	//noter le nombre des etats
	syntax_res->etat_nombre = etats_num;
	lexeme_address += 1;
	return;
}

// analyser mot clef "initial"
void analyse_motclef_initial(lexeme* res)
{
        // examine '='
	lexeme_address += 1;
	is_egale(1, res);
	
	// noter l'info d'initial
	lexeme_address += 1;
	syntax_res->initial = is_entier(res);
	lexeme_address += 1;
	return;
}

// analyser mot clef "final"
void analyse_motclef_final(lexeme* res)
{
	// la forme: final = [0, 1, 2]
	// examine '=' 
	lexeme_address += 1;
	is_egale(1, res);

	// empiler '[' 
	lexeme_address += 1;
	is_parenthese('[', res);

	
	int final_num = 0;
	while (res[lexeme_address + 1].type == Entier)
	{      
	 
	    lexeme_address += 1;
	    int final_etat = atoi(res[lexeme_address].expression);
            syntax_res->final[final_num] = final_etat;
            final_num += 1;
	    lexeme_address += 1;
	    
	    if (res[lexeme_address].type != Symbole){
		is_error(res);
	    }
	    
	    // si ',' on continue, sinon break
	    else if (res[lexeme_address].expression[0] != ',')
		break;
	}

	syntax_res->final_nombre = final_num;
	is_parenthese(']', res);

	lexeme_address += 1;
	return;
}

// analyser mot clef "transitions"
void analyse_motclef_trans(lexeme* res)
{
	int trans_nombre = 0;

	lexeme_address += 1;
	
	// examiner '=' 
	is_egale(1, res);

	// depiler [ 
	lexeme_address += 1;
	is_parenthese('[', res);

	// loop
	while (1)
	{
		trans_info* current_trans = malloc(sizeof(trans_info));
		//la forme : transition = [(0 →  1, 'a', multiple ou non pile), ...] 
                              // empiler '('
		lexeme_address += 1;
		is_parenthese('(', res);

               // noter la source de chaque transition
		lexeme_address += 1;
		current_trans->source = is_entier(res);
               
                // examiner "→"
		lexeme_address += 1;
		if ((res[lexeme_address].type != Fleche) | (strcmp(res[lexeme_address].expression, "→") != 0))
			is_error(res);
                
                // noter la destination de chaque transition
		lexeme_address += 1;
		current_trans->destination = is_entier(res);
                
                // examiner ','
		lexeme_address += 1;
		is_virgule(1, res);
                
                //noter l'emission de chaque transition
		lexeme_address += 1;
		current_trans->emission = is_caractere(res);
                
		lexeme_address += 1;
		if (res[lexeme_address].type == Symbole)
		{
			// il existe des piles
			if (res[lexeme_address].expression[0] == ',')
			{
				lexeme_address += 1;
				search_pile_info(current_trans, res);
			}
			// pas de piles
			else {
				is_parenthese(')', res);
				current_trans->em_ou_de[0] = 0;

			}
		}

		else
		{
			is_error(res);
		}

		syntax_res->trans[trans_nombre] = current_trans;
                trans_nombre += 1;
		lexeme_address += 1;
		if (res[lexeme_address].type == Symbole)
		{       
		        // il existe encore des transitions
			if (res[lexeme_address].expression[0] == ',')
			{
			   
			}
			// la transition est terminé
			else if (res[lexeme_address].expression[0] == ']')
			{
			    break;
			}
			else {
				is_error(res);
			}
		}
		else {
			is_error(res);
		}

	}

	syntax_res->trans_info_nombre = trans_nombre;
        
        // depiler ']'
	is_parenthese(']', res);
	lexeme_address += 1;
	return;
}

void analyse_syntaxique(lexeme* res)
{
        
	int motclef_num = 0;
	// define Mapile
        Mapile.pop = pop_aux;
        Mapile.push = push_aux;
        Mapile.vide = vide_aux;
        
	while (lexeme_address < lexeme_total_length)
	{
		lexeme lex = res[lexeme_address];
		++motclef_num;
		//printf("%c", lex.expression[0]);
		// analyse chaque motclef  	
		if (lex.type == Mot_clef) {
			analyse_motclef(res);
			//printf("%c", lex.expression[0]);
			//printf("%d", lexeme_address);
		}
		else break;
	}
        // Enfin, depiler '}'
	is_parenthese('}', res);
	print_resultat();
}

void print_resultat()
{
	printf("\n-----Syntax Resultat-----");
	printf("\npile_nombre = %d", syntax_res->pile_nombre);
	printf("\netat_nombre = %d", syntax_res->etat_nombre);
	printf("\nétats = ");
	for (int i = 0; i < syntax_res->etat_nombre; ++i)
	{
		printf("%s ", syntax_res->etats[i]);
	}

	printf("\ninitial = %d", syntax_res->initial);
	printf("\nfinal_nombre = %d", syntax_res->final_nombre);
	printf("\nfinal états = ");
	for (int i = 0; i < syntax_res->final_nombre; ++i) {
		printf("%d ", syntax_res->final[i]);
	}

	printf("\n");

	//l'information de transition
	for (int i = 0; i < syntax_res->trans_info_nombre; ++i) {

		printf("\nTransition %d:", i + 1);
		printf("\nsource = %d", syntax_res->trans[i]->source);
		printf("\ndestination = %d", syntax_res->trans[i]->destination);
		printf("\nemission = %c", syntax_res->trans[i]->emission);

		printf("\n");

		// l'information de pile
		for (int j = 0; j < 5; ++j)
		{
			// Si pas de action de empiler ou depiler
			if (syntax_res->trans[i]->em_ou_de[j] == 0)
				continue;

			else
				printf("Pile info: %d : %c ", syntax_res->trans[i]->em_ou_de[j], syntax_res->trans[i]->pile[j]);
		}
		printf("\n");

	}


	printf("\n");
}





