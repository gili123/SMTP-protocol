/*
 * SMTP.c
 * create socket and run smtp
 *
 *  Created on: Dec 20, 2014
 *      Author: Gili Mizrahi  302976840
 */

#include "SMTP.h"


/*
 * create socket to given server and port
 * in success return the socket key
 * in failure return -1
 */
int createSocket(char* server, int portNum){

	int client = 0;
	struct sockaddr_in  serv_addr;
	struct hostent* serv;

	if((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	if((serv = gethostbyname(server)) == NULL)
		return -1;

	bzero((char *) &serv_addr, sizeof(serv_addr));

	bcopy((char *)serv->h_addr, (char *)&serv_addr.sin_addr.s_addr, serv->h_length);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port  = htons(portNum);

	if (connect(client,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		return -1;


	return client;
}


/*
 * method to run the smtp with the given socket
 * if have some problem return 1 else return 0
 */
int smtp(FILE* file, int socket){

	char* suffix;
	char* fileLine;
	char* request;
	char* reply;
	char host[50];

	reply = getReply(socket, "", BY_LINE);
	printf("%s", reply);

	suffix = getSuffix(reply);

	gethostname(host, sizeof(host));

	//write to socket: HELO host
	if((request = (char*)malloc((strlen("HELO ") + strlen(host)) * sizeof(char) + 2)) == NULL)
		return 1;
	strcpy(request, "HELO ");
	strcat(request, host);
	strcat(request, "\n");

	printf("C: %s", request);
	write(socket, request, strlen(request));

	free(reply);
	reply = getReply(socket, suffix, BY_LINE);
	printf("%s", reply);

	//write to socket: MAIL FROM: ...
	bzero(request, strlen(request));
	fileLine = readLineFromFile(file);
	if((request = (char*)realloc(request, ((strlen("MAIL FROM: ") + strlen(fileLine)) * sizeof(char)) + 2)) == NULL)
		return 1;

	strcpy(request, "MAIL FROM: ");
	strcat(request, fileLine);
	strcat(request, "\n");

	write(socket, request, strlen(request));
	printf("C: %s", request);

	free(reply);
	reply = getReply(socket, suffix, BY_SUFFIX);
	printf("%s", reply);

	free(fileLine);

	//write to socket: RCPT TO: ...
	bzero(request, strlen(request));
	fileLine = readLineFromFile(file);
	if((request = (char*)realloc(request, (strlen("RCPT TO: ") + strlen(fileLine)) * sizeof(char) + 2)) == NULL)
		return 1;

	strcpy(request, "RCPT TO: ");
	strcat(request, fileLine);
	strcat(request, "\n");

	write(socket, request, strlen(request));
	printf("C: %s", request);

	free(reply);
	reply = getReply(socket, suffix, BY_SUFFIX);
	printf("%s", reply);

	write(socket, "DATA\n", strlen("DATA\n"));
	printf("C: DATA\n");
	free(reply);
	reply = getReply(socket, suffix, BY_LINE);
	printf("%s", reply);


	//write to socket the rest of lines from the file
	while(1){

		free(fileLine);
		bzero(request, strlen(request));
		fileLine = readLineFromFile(file);

		if(strcmp(fileLine, "EOF") == 0)
			break;


		if((request = (char*)realloc(request, strlen(fileLine) * sizeof(char) + 2)) == NULL)
			return 1;

		strcpy(request, fileLine);
		strcat(request, "\n");

		write(socket, request, strlen(request));
		printf("C: %s", request);
	}

	//write to socket: \r\n.\r\n
	write(socket, "\r\n.\r\n", strlen("\r\n.\r\n"));
	printf("C: .\n");
	free(reply);
	reply = getReply(socket, suffix, BY_SUFFIX);
	printf("%s", reply);

	//write to socket: QUIT
	write(socket, "QUIT\n", strlen("QUIT\n"));
	printf("C: QUIT\n");
	free(reply);
	reply = getReply(socket, suffix, BY_SUFFIX);
	printf("%s", reply);

	free(reply);
	free(suffix);
	free(request);

	return 0;
}


/*
 * read a line from file
 * if success return reference to the line
 * if failure return null
 */
char* readLineFromFile(FILE* file){

	char* line;
	char c;
	int i = 0;

	//count the length of the correct line
	while((c = fgetc(file)) != '\n'){
		if(c == EOF)
			return "EOF";
		i++;
	}

	if((line = (char*)malloc((i + 1) * sizeof(char))) == NULL)
		return NULL;

	fseek(file, -1 * (i + 1), SEEK_CUR);
	fgets(line, i + 1, file);
	fgetc(file);


	return line;
}


/*
 * method to get the reply from the server until the suffix string
 * if success return the reference to that line
 * if failure return null
 */
char* getReply(int socket, char* suffix, int bytype){

	char tmp[50];
	char* reply;
	int nRead = 0, i = 0;

	if((reply = (char*)malloc((strlen("S: ") + 1) * sizeof(char))) == NULL)
		return NULL;

	strcpy(reply, "S: ");

	do{

		bzero(tmp, sizeof(tmp));

		if((nRead = read(socket, tmp, sizeof(tmp) -1)) < 0){
			free(reply);
			return NULL;
		}

		i += nRead;

		if((reply = (char*)realloc(reply, i + strlen("S: ") + 1)) == NULL){
			free(reply);
			return NULL;
		}

		tmp[nRead] = '\0';
		strcat(reply, tmp);


		if(bytype == BY_SUFFIX){
			if(strstr(reply, suffix) != NULL)
				break;
		}

		else if(bytype == BY_LINE){
			if(strstr(reply, "\n") != NULL)
				break;
		}


	}while(1);


	return reply;
}


/*
 * method to find the suffix string from the given string
 * if success return the reference to the suffix
 * if failure return null
 */
char* getSuffix(char* line){

	char* suffix;
	char* tmp;
	int suffix_location = 0;


	if((tmp = (char*)malloc(strlen(line) * sizeof(char) + 1)) == NULL)
		return NULL;

	strcpy(tmp, line);

	suffix_location = strlen(tmp) -2;

	while(tmp[--suffix_location] != ' ');
	suffix_location++;

	if((suffix = (char*)malloc(strlen(line) - suffix_location +1)) == NULL)
		return NULL;

	strcpy(suffix, strtok(tmp + suffix_location, "\0"));
	free(tmp);


	return suffix;
}

