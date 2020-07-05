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
void open_file(char *, char *);
void decrypt(char *, char *, char *);

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead, size1, size2;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char message1[MAX], message2[MAX], message3[MAX];
	char buffer[MAX];
	char *host = "localhost";
	char errmsg1[MAX], errmsg2[MAX];

	strcpy(message3, "d!");

	if (argc < 3) { fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(host); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	open_file(argv[1], message1);
	open_file(argv[2], message2);

	size1 = strlen(message1);
	size2 = strlen(message2);

	if (size1 > size2) {
		fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
		exit(1);
	}


	else{
		// Set up the socket
		socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
		if (socketFD < 0) error("CLIENT: ERROR opening socket");

		// Connect to server
		if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
			error("CLIENT: ERROR connecting");

		memset(buffer, '\0', sizeof(buffer));
		message1[strcspn(message1, "\n")] = '\0';
		message2[strcspn(message2, "\n")] = '\0';
		strcat(message1, "#");
		strcat(message1, message2);
		strcat(message3, message1);
	
		// Send message to server
		charsWritten = send(socketFD, message3, strlen(message3), 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(message3)) printf("CLIENT: WARNING: Not all data written to socket!\n");

		// Get return message from server
		memset(buffer, '\0', 256); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, 255, 0); // Read data from the socket, leaving \0 at end

		if(strcmp(buffer,"NONE")==0){
			fprintf(stderr,"Error: could not contact otp_dec_d on port %s\n",argv[3]);
			exit (2);
		}
		else{
		if (charsRead < 0) error("CLIENT: ERROR reading from socket");
		printf("%s\n", buffer);
		}
		close(socketFD); // Close the socket
	}
	return 0;
}

void open_file(char *file, char *message) {
	int fd;
	char readBuffer[MAX];
	size_t nread;

	memset(readBuffer, 0, sizeof(readBuffer));
	memset(message, 0, sizeof(message));

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open file %s\n", file);
		exit(1);
	}
	lseek(fd, 0, SEEK_SET);
	nread = read(fd, readBuffer, sizeof(readBuffer));

	strcpy(message, readBuffer);

}




