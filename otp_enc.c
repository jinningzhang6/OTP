#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>

#define MAX 500000

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
void open_file(char *,char *);
void check_port(char *, char *,int);

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead,size1,size2;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char message1[MAX],message2[MAX],message3[MAX],portm[MAX];
	char buffer[MAX];
	char *host="localhost";
	char errmsg1[MAX], errmsg2[MAX];
	char newport[MAX];
	int realport;

	strcpy(message3,"e!");

	//strcpy(portm,argv[4]);
	//check_port(newport,portm,realport);

	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(host); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	open_file(argv[1],message1);
	open_file(argv[2],message2);

	size1=strlen(message1);
	size2=strlen(message2);

	if (size1>size2){
		fprintf(stderr,"Error: key '%s' is too short\n",argv[2]);
		memset(buffer,'\0',sizeof(buffer));
		exit (1);
	}

	else{
		memset(buffer,'\0',sizeof(buffer));
		message1[strcspn(message1, "\n")]='\0';
		message2[strcspn(message2, "\n")]='\0';
		strcat(message1,"#");
		strcat(message1,message2);
		strcat(message3,message1);
	
		// Send message to server
		charsWritten = send(socketFD, message3, strlen(message3), 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(message3)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	
		memset(buffer, '\0', 256);
		charsRead = recv(socketFD, buffer, 255, 0); // Read the client's message from the socket
		
		if (charsRead < 0) error("ERROR reading from socket");
		
		if(strcmp(buffer,"NONE")==0){
			fprintf(stderr,"Error: could not contact otp_enc_d on port %s\n",argv[3]);
			exit (2);
		}
		else{
			printf("%s\n",buffer);	
		}
		close(socketFD);
	}
	return 0;
}

void open_file(char *file,char *message){
	int fd;
	int i;
	char readBuffer[MAX];
	size_t nread;

	memset(readBuffer,0,sizeof(readBuffer));
	memset(message,0,sizeof(message));

	fd= open(file,O_RDONLY);
	if (fd<0){
		fprintf(stderr, "Could not open file %s\n",file);
		exit(1);}
	lseek(fd,0,SEEK_SET);
	nread= read(fd, readBuffer,sizeof(readBuffer));

	for(i=1;i<strlen(readBuffer);i++){
		if(readBuffer[i]=='#'){
			fprintf(stderr, "otp_enc error: input contains bad characters\n");;
			exit(1);}
		else{
			}
	}

	strcpy(message,readBuffer);

}

