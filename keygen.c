#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX 27
#define max 600000

void generate_rand(char *, size_t);

int main(int argc, char *argv[]){
	srand(time(NULL));
	if (argc<2){ printf("Error! Invalid argument size!\n"); exit(0);}
	int size;
	char str[max];
	size= atoi(argv[1]);
	//printf("number: %d",size);
	if (!size){ printf("Error! 2nd argument should be an integer\n"); exit(0);}
	memset(str,'\0',max);


	if (argc==3){
		printf("Error! Invalid input!\n"); exit(0);}

	if (argc >3){
	if ((strcmp(argv[2],">")==0) && (argv[3]!=NULL)){
		printf("yes");
	}

	else if ((strcmp(argv[2],">")==0) && (argv[3]==NULL)){
		printf("Error! Missing input for redirection!\n"); exit(0);
	}
	else {
		printf("Error! Invalid input!\n"); exit(0);
	}
	}

	generate_rand(str,size);

}

void generate_rand(char *dest, size_t length){
	int i;
	int key=0;
	char final[length];

	memset(dest,'\0',length);
	memset(final,'\0',length);

	char whole[MAX] = { ' ','A', 'B', 'C', 'D', 'E', 'F', 'G', 
                          'H', 'I', 'J', 'K', 'L', 'M', 'N',  
                          'O', 'P', 'Q', 'R', 'S', 'T', 'U', 
                          'V', 'W', 'X', 'Y', 'Z'};

	for(i=0;i<length;i++){
		key= rand() % (sizeof(whole)-1);
		dest[i] = whole[key];
		printf("%c",dest[i]);
	}
	printf("\n");
	/*strncpy(final,dest,length);
	memset(dest,'\0',length);
	strcpy(dest,final);*/
}
