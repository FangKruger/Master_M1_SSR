/*
*
*SEL TP
*Author: Manon Derocles & Fanny Prieur
*
*/



#include "tp1.h"


//******************************** trouverPid *************************//
int trouverPid(char *progr){

	char cmd[MAX_LENGTH];//contient la commande permetant d'obtenir le PID
	int pGrep;//recupère le statut de la commande. -1 si faux
	FILE *proc; //fichier contenant le PID
	char npid[MAX_LENGTH];//contient le pid
	int num_pid;//contient le pid, utilisé pour la fonction atoi pour passer d'une chaine a un int (npid->pid)


	//cmd = est la commande qui permet d'obtenir le numero de pid du programme demandé
	snprintf(cmd,sizeof("pgrep  > proc.txt")+sizeof(progr), "pgrep %s > proc.txt", progr);

	//transmet le nom de la commande ou le nom du programme spécifié par la commande à l'environnement hôte à exécuter par le processeur de commandes et renvoie une fois la commande terminée.
	pGrep = system(cmd);
	pGrep = WEXITSTATUS(pGrep);//convertit le resultat de retour obtenu par system pour le comparé si il y a une erreur

	if(pGrep==1){
		perror("systeme erreur \n");
		return -1;
	}

	//on ouvre le fichier ou le pid se trouve
	proc= fopen("proc.txt","r");

	if (proc == NULL){
		perror("fopen erreur pour proc.txt\n");
		return -1;
	}

	if(fread(npid,sizeof(char),16, proc)==0){// on recupere le pid qui est dans le fichier et on le met dans npid
		perror("fread erreur pid \n");
		return -1;
	}

	//conversion chaine en int
	num_pid= atoi(npid);

	if(num_pid <0){
		perror("Erreur pgrep pour le processus demander \n");
		return -1;
	}

	if(fclose(proc)<0){//fermer le fichier
		perror("erreur sur la fermeture de proc");
		return -1;
	}

	return num_pid;
}

//******************************** attacherProc *************************//
void attacherProc(int num_pid){

	long attache;

	// ptrace attache le proc grace a son numero de pid preablement recuperé

	attache = ptrace(PTRACE_ATTACH, num_pid,0,0);

	if(attache < 0){
		perror("Erreur processus non attaché\n");
		exit(0);
	}
	wait(&num_pid);/// retourne le pid du processus fils si le retour est dû à la terminaison d'un processus fils ; -1 en cas d'erreur

}


//******************************** trouverAdrMemoire *************************//
long unsigned trouverAdrMemoire (char *progr,char *fct){

	char cmd2[MAX_LENGTH];//chaine de caractère contenant la commande permettant de trouver l'adresse mémoire
	int nm;//stock le retour de l'appel systeme de la commande de récuparation d'adresse
	FILE *adrMemoir;//fichier contenant l'adresse mémoire
	long unsigned adrMem;
	char str1[MAX_LENGTH];
	char str2[MAX_LENGTH];


	//cmd2= est la commande qui nous sert à trouver l'@ mermoire n en fait nm donne la table des symboles du programme et avec grep on reduit la recherche de la fonction qu'on veut
	snprintf(cmd2,sizeof("nm ")+sizeof(progr)+1 + sizeof(" | grep ") +sizeof(fct)+1 +sizeof(" > adresseMemoire.txt"),"nm %s | grep %s > adresseMemoire.txt", progr,fct);

	nm = system(cmd2);// execute la commande
	// traduit en un truc comprehensible pour la verification d'erreur -> renvoie le code de retour du processus fils passé à _exit() ou exit() ou la valeur retournée par la fonction main()
	nm = WEXITSTATUS(nm);

	if(nm == 1){
		perror("popen erreur \n");
		return -1;
	}
	//on ouvre le fichier ou l'@ memoire a eté rangé
	adrMemoir = fopen("adresseMemoire.txt","r");

	if (adrMemoir == NULL){
		perror("fopen erreur adresseMemoire.txt\n");
		return -1;
	}

	fscanf(adrMemoir,"%lx %s %s", &adrMem,str1,str2);//on parse la ligne de string qui est contenu dans ce fichier et on veut l'entité qui correspond un long et suit en tete de ligne


	if(fclose(adrMemoir)<0){
		perror("erreur sur la fermeture de adrmemoir");
		return -1;
	}

	return adrMem;
}


//******************************** modifMem *************************//
void modifMemStopper(int num_pid,long unsigned adrMem){
	char stop ={0xCC};
	FILE *chemin;
	char cmd3[MAX_LENGTH];

	// cmd3= est la commande qui permet d'avoir acces a la memoire du programme dans le but de la modifier par la suite
	snprintf(cmd3,sizeof("/proc//mem")+sizeof(num_pid)+1,"/proc/%d/mem",num_pid);


	chemin = fopen(cmd3,"w");// ouvre ce fichier system
	if (chemin == NULL){
		perror("fopen erreur pour ouverture du chemin /proc/pid/mem \n");
		exit(0);
	}


	if(fseek(chemin,adrMem,SEEK_SET)<0){//chemin = pointeur qui pointe excatement sur l'@ memoire de notre fct a modifier
		perror("fseek erreur\n");
		exit(0);
	}

	if(fwrite(&stop,1,1,chemin)<0){ // on ecrit le code assemble qui veut interruption dans cette memoire
		perror("fwrite erreur\n");
		exit(0);
	}

	if(fclose(chemin)<0){//ferme le fichier
		perror("erreur sur la fermeture de chemin");
		exit(0);
	}

}


//*************************************************************************MAIN****************************************************************************************************************

int main(int argc, char* argv[]){

	char *progr = argv[1];
	char *fct = argv[2];
	int num_pid;
	long unsigned adrMem;


////////////////////////////////////////////////////////////Challenge 1/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (progr == NULL || fct == NULL){
		printf("Il faut mettre deux parametre, programme et fonction \n");
		return -1;
	}

	//********************************Partie recuperation du pid*************************//
	num_pid = trouverPid(progr);
	if(attacherProc(num_pid)<0){
		return -1;
	}


	//********************************Partie adresses mémoire*************************//
	adrMem = trouverAdrMemoire(progr,fct);
	if(modifMemStopper(num_pid,adrMem)<0){
		return -1;
	}

	printf("succes de Challenge 1 \n");



	return 0;
	
}
