#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX 80000
#define MIN 70000

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
void encrypt(char *,char *,char *);
void splitstring(char *,char *,char *);

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead,charsWritten;
	socklen_t sizeOfClientInfo;
	int loop=0;
	char buffer[256], errmsg1[MAX], errmsg2[MAX];
	char m1[MAX], m2[MAX], enp[MAX];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

	while(loop<5){
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	pid_t spawnpid= -5;
	spawnpid= fork();
	switch (spawnpid)
	{
	case -1:
		printf("Error: could not contact otp_enc_d on port %s\n",argv[4]);
		exit (2);

	case 0:
	// Get the message from the client and display it
	memset(buffer, '\0', 256);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket

	if (charsRead < 0) error("ERROR reading from socket");
	if(buffer!=NULL){
		if(buffer[0] != 'e'){
			strcpy(enp,"NONE");
		}
		else{
		memset(enp,'\0',sizeof(enp));
		splitstring(buffer,m1,m2);
		encrypt(m1,m2,enp);
		}
	}

	charsWritten = send(establishedConnectionFD, enp, strlen(enp), 0); // Write to the Client
	if (charsWritten < 0) error("Server: ERROR writing to socket");
	if (charsWritten < strlen(enp)) printf("Server: WARNING: Not all data written to socket!\n");
	loop++;

	default:
		break;
		loop++;
	}
	}
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0; 
}

void splitstring(char *str,char *msg1,char *msg2){
	memset(msg1,'\0',sizeof(msg1));
	memset(msg2,'\0',sizeof(msg2));

	int i=1;
	char* token[MAX];
	token[0] = strtok(str,"#");
	while(1){
		if (token[i]=NULL){
			break;
		}
		else{
			token[i]=strtok(NULL,"#");
			i++;
		}
	}
	strncpy(msg1,token[0]+2,strlen(token[0]));
	strcpy(msg2,token[1]);
	msg2[strlen(msg2)]='\0';

	memset(str,'\0',sizeof(str));
	
}



void encrypt(char *msg1,char *msg2,char *final){
	int i,j,k,m,n;
	char x;
	int msglength=strlen(msg1)+1;
	msg1[strcspn(msg1,"\n")] = '\0';
	msg2[strcspn(msg2,"\n")] = '\0';

	char whole[MAX] = {'A', 'B', 'C', 'D', 'E', 'F', 'G',
                           'H', 'I', 'J', 'K', 'L', 'M', 'N',
                           'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                           'V', 'W', 'X', 'Y', 'Z', ' '};
	int array1[msglength];
	int array2[msglength];
	char array3[MAX];

	for(i=0;i<msglength;i++){	/*iterate through loop*/
		for(j=0;j<strlen(whole);j++){ /*find same char and get int num of char*/
			if(whole[j]==msg1[i]){ /*compare char and store the num in the array*/
				array1[i]=j;	}
			if(whole[j]==msg2[i]){ /*compare char, store in the next array*/
				array2[i]=j;	}
		}
	}

	for(k=0;k<msglength-1;k++){
		m=array1[k]+array2[k];
		if(m>26){ m= m % 27;}
		array3[k]=whole[m];
	}
	strcpy(final,array3);
}
