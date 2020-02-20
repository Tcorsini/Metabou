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


struct TProblem							//**D�finition du probl�me:
{
	std::string Nom;					//**Nom du fichier de donn�es
	int NbTache;						//**Nombre de t�ches
	std::vector<int> Temps;				//**Temps de r�alisation pour effectuer une t�che. NB: Tableau de 0 � NbTache-1.  
	std::vector<int> PenAv;				//**P�nalit� encourue par unit� de temps si une t�che est r�alis�e d'avance. NB: Tableau de 0 � NbTache-1.
	std::vector<int> PenRe;				//**P�nalit� encourue par unit� de temps si une t�che est r�alis�e en retard. NB: Tableau de 0 � NbTache-1.
	int DateDue;						//**Date due commune pour l'ensemble des t�ches
	int Somme;							//**Somme des temps de r�alisation pour l'ensemble des t�ches (pour calculer date due commune)
	double H;							//**Param�tre du probl�me permettant de calculer la date d'�ch�ance (due) commune
};

struct TSolution						//**D�finition d'une solution: 
{
	std::vector<int> Seq;				//**Ensemble de g�nes: indique la s�quence de r�alisation des t�ches. NB: Tableau de 0 � NbTache-1.
	long FctObj;						//**Valeur de la fonction obj: Sommation des p�nalit�s de retard et d'avance
	std::vector<int> Fin;				//**Temps de terminaison d'une t�che dans la s�quence
	std::vector<int> TAvance;			//**Indique le nombre d'unit�s de temps qu'une t�che est termin�e d'avance
	std::vector<int> TRetard;			//**Indique le nombre d'unit�s de temps qu'une t�che est termin�e en retard
	std::vector<int> PAvance;			//**Calcul de la p�nalit� d'avance pour une t�che (PAvance * TAvance)
	std::vector<int> PRetard;			//**Calcul de la p�nalit� de retard pour une t�che (PRetard * TRetard)
};

struct Transformation  //D�finition d'une op�ration de transformation du voisinage
{
	Transformation(int a = 0, int b = 0) : posA(a), posB(b) {}

	int posA;
	int posB;
	inline bool operator==(const Transformation& other) const {
		return ((posA == other.posA && posB == other.posB) || (posA == other.posB && posB == other.posA));
	}
};

struct TAlgo  //D�finition du Tabou
{
	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	int		TailleVoisinage;			//**Nombre de solutions voisines g�n�r�es � chaque it�ration						//***NEW***
	int		LngListeTabous;				//**Longueur de la liste des tabous													//***NEW***
	std::list<Transformation> ListeTabous;

	Transformation DerniereTransfo; //argumen temporaire, repr�sente la derni�re transformation employ�e

	bool isTabou(const Transformation& t) {
		for (const Transformation& t2 : ListeTabous) {
			if (t2 == t) return true;
		}
		return false;
	}
};

#endif