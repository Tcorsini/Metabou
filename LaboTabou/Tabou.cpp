
#include "Entete.h"
#pragma comment (lib,"TabouDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (TabouDLL.dll et TabouDLL.lib) doivent 
//se trouver dans le r�pertoire courant du projet pour une ex�cution � l'aide du compilateur. Indiquer les
//arguments du programme dans les propri�t�s du projet - d�bogage - arguements.
//Sinon, utiliser le r�pertoire execution.

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem & unProb, TAlgo &unAlgo);

//DESCRIPTION:	Fonction d'affichage � l'�cran permettant de voir si les donn�es du fichier probl�me ont �t� lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme (TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'�cran pour validation (avec ou sans d�tails des calculs)
extern "C" _declspec(dllimport) void AfficherSolution(const TSolution uneSolution, TProblem unProb, std::string Titre, bool AvecCalcul);

//DESCRIPTION: Affichage � l'�cran de la solution finale, du nombre d'�valuations effectu�es et de certains param�tres
extern "C" _declspec(dllimport) void AfficherResultats (const TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier (en append) de la solution finale, du nombre d'�valuations effectu�es et de certains param�tres
extern "C" _declspec(dllimport) void AfficherResultatsFichier (const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	�valuation de la fonction objectif d'une solution et MAJ du compteur d'�valuation. 
//				Fonction objectif repr�sente le retard total pond�r�
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution &uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION:	Cr�ation d'une s�quence al�atoire de parcours des villes et �valuation de la fonction objectif. Allocation dynamique de m�moire pour la s�quence (.Seq)
extern "C" _declspec(dllimport) void CreerSolutionAleatoire(TSolution & uneSolution, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: Copie de la s�quence et de la fonction objectif dans une nouvelle TSolution. La nouvelle TSolution est retourn�e.
extern "C" _declspec(dllimport) void CopierSolution (const TSolution uneSol, TSolution &Copie, TProblem unProb);

//DESCRIPTION:	Lib�ration de la m�moire allou�e dynamiquement
extern "C" _declspec(dllimport) void	LibererMemoireFinPgm	(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb);

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Cr�ation d'une solution voisine � partir de la solution uneSol. NB:uneSol ne doit pas �tre modifi�e
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION:	 Echange de deux villes s�lectionn�e al�atoirement. NB:uneSol ne doit pas �tre modifi�e
TSolution	AppliquerVoisinage	(const TSolution uneSol, TProblem unProb, TAlgo &unAlgo);


//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	TSolution Courante;		//Solution active au cours des it�rations
	TSolution Next;			//Solution voisine retenue � une it�ration
	TSolution Best;			//Meilleure solution depuis le d�but de l'algorithme //***NEW 
	TProblem LeProb;		//D�finition de l'instance de probl�me
	TAlgo LAlgo;			//D�finition des param�tres de l'agorithme
	string NomFichier;
		
	//**Lecture des param�tres
	NomFichier.assign(Param[1]);
	LeProb.H = atof(Param[2]);
	
	LAlgo.TailleVoisinage = atoi(Param[3]);
	LAlgo.LngListeTabous = atoi(Param[4]);
	
	LAlgo.NB_EVAL_MAX = atoi(Param[5]);
	LAlgo.Best = &Best;

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);
	
	//**Cr�ation de la solution initiale 
	CreerSolutionAleatoire(Courante, LeProb, LAlgo);
	Best = Courante;
	AfficherSolution(Courante, LeProb, "SolInitiale: ", false);

	do
	{
		Courante = GetSolutionVoisine(Courante, LeProb, LAlgo);
		//AfficherSolution(Courante, LeProb, "Courante: ", false);
		//AfficherSolution(Next, LeProb, "Next: ", false);
		if (LAlgo.ListeTabous.size() >= LAlgo.LngListeTabous)
			LAlgo.ListeTabous.pop_back();
		LAlgo.ListeTabous.push_front(LAlgo.DerniereTransfo);

		if (Courante.FctObj < Best.FctObj) {
			CopierSolution(Courante, Best, LeProb);
			cout << "CptEval: " << LAlgo.CptEval << "\tFct Obj Nouvelle Best: " << Best.FctObj << endl;
		}
	}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX);
	AfficherResultats(Best, LeProb, LAlgo);
	AfficherResultatsFichier(Best, LeProb, LAlgo,"Resultats.txt");
	
	LibererMemoireFinPgm(Courante, Next, Best, LeProb);

	system("PAUSE");
	return 0;
}

//DESCRIPTION: Cr�ation d'une solution voisine � partir de la solution uneSol qui ne doit pas �tre modifi�e.
//Dans cette fonction, on appel le TYPE DE VOISINAGE s�lectionn� + on d�termine la STRAT�GIE D'ORIENTATION. 
//Ainsi, si la R�GLE DE PIVOT n�cessite l'�tude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage" sera appel�e plusieurs fois.
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo &unAlgo)
{
	//Type (structure) de voisinage : �change de 2 taches s�lectionn�es al�atoirement
	//Parcours dans le voisinage : Aleatoire
	//R�gle de pivot : k-ImproveBEST (k �tant donn� en param�tre pour l'ex�cution du pgm)

	TSolution unVoisin;
	TSolution unGagnant;
	Transformation bestTranfo;
	int i;

	//AfficherSolution(uneSol, unProb, "Courante: ", false);
	unGagnant = AppliquerVoisinage(uneSol, unProb, unAlgo);
	//AfficherSolution(uneSol, unProb, "CouranteApres: ", false);
	//AfficherSolution(unGagnant, unProb, "unGagnant: ", false);

	for (i = 2; i <= unAlgo.TailleVoisinage; i++)
	{
		unVoisin = AppliquerVoisinage(uneSol, unProb, unAlgo);
		if (unAlgo.isTabou(unAlgo.DerniereTransfo)) //cas tabou
			if(unVoisin.FctObj >= unAlgo.Best->FctObj) //on checke crit�re aspiration minimal (est-ce mieux que la meilleure solution)
				continue;
		if (unVoisin.FctObj < unGagnant.FctObj) {
			unGagnant = unVoisin;
			bestTranfo = unAlgo.DerniereTransfo;
		}
		else
			if (unVoisin.FctObj == unGagnant.FctObj)
			{
				if (rand() % 2 == 0) {
					unGagnant = unVoisin;
					bestTranfo = unAlgo.DerniereTransfo;
				}
			}
	}
	unVoisin.Seq.clear();
	unAlgo.DerniereTransfo = bestTranfo;
	return (unGagnant);
}

//DESCRIPTION: Fonction appliquant le type de voisinage s�lectionn�. La fonction retourne la solution voisine obtenue suite � l'application du type de voisinage.
TSolution AppliquerVoisinage (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo)
{
	//DESCRIPTION: Echange de deux commandes s�lectionn�es al�atoirement
	TSolution Copie;
	int PosA, PosB, Tmp;

	//Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	CopierSolution(uneSol, Copie, unProb);
	
	//Tirage al�atoire des 2 commandes
	PosA = rand() % unProb.NbTache;
	do
	{
		PosB = rand() % unProb.NbTache;
	}while (PosA == PosB); //Validation pour ne pas consommer une �valuation inutilement
	
	//Echange des 2 commandes
	Tmp = Copie.Seq[PosA];
	Copie.Seq[PosA] = Copie.Seq[PosB];
	Copie.Seq[PosB] = Tmp;
	
	unAlgo.DerniereTransfo = Transformation(PosA, PosB);
	//Le nouveau voisin doit �tre �valu� 
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}
