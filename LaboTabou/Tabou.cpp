
#include "Entete.h"
#pragma comment (lib,"TabouDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (TabouDLL.dll et TabouDLL.lib) doivent 
//se trouver dans le répertoire courant du projet pour une exécution à l'aide du compilateur. Indiquer les
//arguments du programme dans les propriétés du projet - débogage - arguements.
//Sinon, utiliser le répertoire execution.

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem & unProb, TAlgo &unAlgo);

//DESCRIPTION:	Fonction d'affichage à l'écran permettant de voir si les données du fichier problème ont été lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme (TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'écran pour validation (avec ou sans détails des calculs)
extern "C" _declspec(dllimport) void AfficherSolution(const TSolution uneSolution, TProblem unProb, std::string Titre, bool AvecCalcul);

//DESCRIPTION: Affichage à l'écran de la solution finale, du nombre d'évaluations effectuées et de certains paramètres
extern "C" _declspec(dllimport) void AfficherResultats (const TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier (en append) de la solution finale, du nombre d'évaluations effectuées et de certains paramètres
extern "C" _declspec(dllimport) void AfficherResultatsFichier (const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Évaluation de la fonction objectif d'une solution et MAJ du compteur d'évaluation. 
//				Fonction objectif représente le retard total pondéré
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution &uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION:	Création d'une séquence aléatoire de parcours des villes et évaluation de la fonction objectif. Allocation dynamique de mémoire pour la séquence (.Seq)
extern "C" _declspec(dllimport) void CreerSolutionAleatoire(TSolution & uneSolution, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION: Copie de la séquence et de la fonction objectif dans une nouvelle TSolution. La nouvelle TSolution est retournée.
extern "C" _declspec(dllimport) void CopierSolution (const TSolution uneSol, TSolution &Copie, TProblem unProb);

//DESCRIPTION:	Libération de la mémoire allouée dynamiquement
extern "C" _declspec(dllimport) void	LibererMemoireFinPgm	(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb);

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Création d'une solution voisine à partir de la solution uneSol. NB:uneSol ne doit pas être modifiée
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION:	 Echange de deux villes sélectionnée aléatoirement. NB:uneSol ne doit pas être modifiée
TSolution	AppliquerVoisinage	(const TSolution uneSol, TProblem unProb, TAlgo &unAlgo);


//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	TSolution Courante;		//Solution active au cours des itérations
	TSolution Next;			//Solution voisine retenue à une itération
	TSolution Best;			//Meilleure solution depuis le début de l'algorithme //***NEW 
	TProblem LeProb;		//Définition de l'instance de problème
	TAlgo LAlgo;			//Définition des paramètres de l'agorithme
	string NomFichier;
		
	//**Lecture des paramètres
	NomFichier.assign(Param[1]);
	LeProb.H = atof(Param[2]);
	
	LAlgo.TailleVoisinage = atoi(Param[3]);
	LAlgo.LngListeTabous = atoi(Param[4]);
	
	LAlgo.NB_EVAL_MAX = atoi(Param[5]);
	LAlgo.Best = &Best;

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);
	
	//**Création de la solution initiale 
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

//DESCRIPTION: Création d'une solution voisine à partir de la solution uneSol qui ne doit pas être modifiée.
//Dans cette fonction, on appel le TYPE DE VOISINAGE sélectionné + on détermine la STRATÉGIE D'ORIENTATION. 
//Ainsi, si la RÈGLE DE PIVOT nécessite l'étude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage" sera appelée plusieurs fois.
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo &unAlgo)
{
	//Type (structure) de voisinage : Échange de 2 taches sélectionnées aléatoirement
	//Parcours dans le voisinage : Aleatoire
	//Règle de pivot : k-ImproveBEST (k étant donné en paramètre pour l'exécution du pgm)

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
			if(unVoisin.FctObj >= unAlgo.Best->FctObj) //on checke critère aspiration minimal (est-ce mieux que la meilleure solution)
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

//DESCRIPTION: Fonction appliquant le type de voisinage sélectionné. La fonction retourne la solution voisine obtenue suite à l'application du type de voisinage.
TSolution AppliquerVoisinage (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo)
{
	//DESCRIPTION: Echange de deux commandes sélectionnées aléatoirement
	TSolution Copie;
	int PosA, PosB, Tmp;

	//Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	CopierSolution(uneSol, Copie, unProb);
	
	//Tirage aléatoire des 2 commandes
	PosA = rand() % unProb.NbTache;
	do
	{
		PosB = rand() % unProb.NbTache;
	}while (PosA == PosB); //Validation pour ne pas consommer une évaluation inutilement
	
	//Echange des 2 commandes
	Tmp = Copie.Seq[PosA];
	Copie.Seq[PosA] = Copie.Seq[PosB];
	Copie.Seq[PosB] = Tmp;
	
	unAlgo.DerniereTransfo = Transformation(PosA, PosB);
	//Le nouveau voisin doit être évalué 
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}
