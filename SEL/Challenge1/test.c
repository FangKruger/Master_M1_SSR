#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void compteurSeconde(){
	int i=0;
	while (i<60){
		printf("%d secondes\n",i);
		i++;
		sleep(1);
	}
}

void compteurMinute(int j){
	printf("%d minutes et\n",j);
}

int main(){
	int j=0;
	while(1){
		compteurMinute(j);
		compteurSeconde();
		j++;
	}
return 0;
}
