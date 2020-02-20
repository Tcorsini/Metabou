#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
#include <list>

using namespace std;


struct TProblem							//**Définition du problème:
{
	std::string Nom;					//**Nom du fichier de données
	int NbTache;						//**Nombre de tâches
	std::vector<int> Temps;				//**Temps de réalisation pour effectuer une tâche. NB: Tableau de 0 à NbTache-1.  
	std::vector<int> PenAv;				//**Pénalité encourue par unité de temps si une tâche est réalisée d'avance. NB: Tableau de 0 à NbTache-1.
	std::vector<int> PenRe;				//**Pénalité encourue par unité de temps si une tâche est réalisée en retard. NB: Tableau de 0 à NbTache-1.
	int DateDue;						//**Date due commune pour l'ensemble des tâches
	int Somme;							//**Somme des temps de réalisation pour l'ensemble des tâches (pour calculer date due commune)
	double H;							//**Paramètre du problème permettant de calculer la date d'échéance (due) commune
};

struct TSolution						//**Définition d'une solution: 
{
	std::vector<int> Seq;				//**Ensemble de gènes: indique la séquence de réalisation des tâches. NB: Tableau de 0 à NbTache-1.
	long FctObj;						//**Valeur de la fonction obj: Sommation des pénalités de retard et d'avance
	std::vector<int> Fin;				//**Temps de terminaison d'une tâche dans la séquence
	std::vector<int> TAvance;			//**Indique le nombre d'unités de temps qu'une tâche est terminée d'avance
	std::vector<int> TRetard;			//**Indique le nombre d'unités de temps qu'une tâche est terminée en retard
	std::vector<int> PAvance;			//**Calcul de la pénalité d'avance pour une tâche (PAvance * TAvance)
	std::vector<int> PRetard;			//**Calcul de la pénalité de retard pour une tâche (PRetard * TRetard)
};

struct Transformation  //Définition d'une opération de transformation du voisinage
{
	Transformation(int a = 0, int b = 0) : posA(a), posB(b) {}

	int posA;
	int posB;
	inline bool operator==(const Transformation& other) const {
		return ((posA == other.posA && posB == other.posB) || (posA == other.posB && posB == other.posA));
	}
};

struct TAlgo  //Définition du Tabou
{
	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	int		TailleVoisinage;			//**Nombre de solutions voisines générées à chaque itération						//***NEW***
	int		LngListeTabous;				//**Longueur de la liste des tabous													//***NEW***
	std::list<Transformation> ListeTabous;

	Transformation DerniereTransfo; //argumen temporaire, représente la dernière transformation employée

	bool isTabou(const Transformation& t) {
		for (const Transformation& t2 : ListeTabous) {
			if (t2 == t) return true;
		}
		return false;
	}
};

#endif