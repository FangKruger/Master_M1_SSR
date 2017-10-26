#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

// TODO .h + var golbal
int trouverPid(char *progr){
	char cmd[512];
	int pGrep;
	FILE *proc;
	char npid[512];
	int num_pid;
	
	//printf("programme %s \n",progr);
	snprintf(cmd,sizeof("pgrep  > proc.txt")+sizeof(progr), "pgrep %s > proc.txt", progr);

	pGrep = system(cmd);
	pGrep = WEXITSTATUS(pGrep);
	
	if(pGrep==1){
		perror("system erreur \n");
		return -1;
	}

	//recupere le pid avec fopen sinon si juste system c'est le code de retour de system que l'on recupere
	proc= fopen("proc.txt","r");

	if (proc == NULL){
		perror("fopen erreur pour proc.txt\n");
		return -1;
	}

	if(fread(npid,sizeof(char),16, proc)==0){
		perror("fread erreur pid \n");	
		return -1;	
	}
	
	num_pid= atoi(npid);
	
	if(fclose(proc)<0){
		perror("erreur sur la fermeture de proc");
		return -1;
	}
	
	return num_pid;
}

int attacherProc(int num_pid){
	
	long attache;
	if(num_pid <0){
		perror("Erreur pgrep pour le processus demander \n");
		return -1;
	}

	//printf("le numero du proc est :%d \n\n",num_pid);

	attache = ptrace(PTRACE_ATTACH, num_pid,0,0);
	
	if(attache < 0){
		perror("Erreur processus non attaché\n");
		return -1;
	}
	wait(&num_pid);
	return 1;
	}
	
long unsigned trouverAdrMemoire (char *progr,char *fct){
		
	char cmd2[1024];
	int nm;
	FILE *adrMemoir;
	long unsigned adrMem;
	char str1[512];
	char str2[512];
	
	
	//printf("on prends l'adresse memoire de la fonction: %s du programme %s  \n",fct,progr);
	snprintf(cmd2,sizeof("nm ")+sizeof(progr)+1 + sizeof(" | grep ") +sizeof(fct)+1 +sizeof(" > adresseMemoire.txt"),"nm %s | grep %s > adresseMemoire.txt", progr,fct);

	nm = system(cmd2);
	nm = WEXITSTATUS(nm);
	
	if(nm == 1){
		perror("popen erreur \n");
		return -1;
	}
	adrMemoir = fopen("adresseMemoire.txt","r");

	if (adrMemoir == NULL){
		perror("fopen erreur adresseMemoire.txt\n");
		return -1;
	}

	fscanf(adrMemoir,"%lx %s %s", &adrMem,str1,str2);//adrmem contient l'adresse memoire
	//printf("l'adresse est : %lx \n\n", adrMem);
	
	if(fclose(adrMemoir)<0){
		perror("erreur sur la fermeture de adrmemoir");
		return -1;
	}
	
	return adrMem;
}

int modifMem(int num_pid,long unsigned adrMem){
	char stop ={0xCC};
	FILE *chemin;
	
	char cmd3[512];
	snprintf(cmd3,sizeof("/proc//mem")+sizeof(num_pid)+1,"/proc/%d/mem",num_pid);

	
	chemin = fopen(cmd3,"w");
	if (chemin == NULL){
		perror("fopen erreur pour ouverture du chemin /proc/pid/mem \n");
		return -1;
	}
	
	
	if(fseek(chemin,adrMem,SEEK_SET)<0){
		perror("fseek erreur\n");
		return -1;	
	}

	if(fwrite(&stop,1,1,chemin)<0){
		perror("fwrite erreur\n");
		return -1;
	}	
	
	if(fclose(chemin)<0){
		perror("erreur sur la fermeture de chemin");
		return -1;
	}
	return 1;
	
}


int main(int argc, char* argv[]){
	
	int num_pid;
	
	long unsigned adrMem;

	char *progr = argv[1];
	char *fct = argv[2];	

	if (progr == NULL || fct == NULL){
		printf("Il faut mettre deux parametre, programme et fonction \n");
		return -1;
	}

	/////////////////////Parti de recuperer le pid
	num_pid = trouverPid(progr);
	
	if (attacherProc(num_pid)<0){
		return -1;
	}
	
	/////////////////////////////////// PARTI DES ADRESSE MEMOIRE
	adrMem = trouverAdrMemoire(progr,fct);

	///////////////////////////////Suite
	
	if (modifMem(num_pid,adrMem)<0){
		return -1;
	}

	printf("succes");

	return 0;
}
