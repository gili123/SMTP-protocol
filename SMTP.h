/*
 * SMTP.h
 * create socket and run smtp
 *
 *  Created on: Dec 20, 2014
 *      Author: Gili Mizrahi  302976840
 */

#ifndef SMTP_H_
#define SMTP_H_

#define BY_LINE 0
#define BY_SUFFIX 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/*
 * create socket to given server and port
 * in success return the socket key
 * in failure return -1
 */
int createSocket(char* server, int portNum);

/*
 * method to run the smtp with the given socket
 * if have some problem return 1 else return 0
 */
int smtp(FILE* file, int socket);

/*
 * read a line from file
 * if success return reference to the line
 * if failure return null
 */
char* readLineFromFile(FILE* file);

/*
 * method to get the reply from the server until the suffix string
 * if success return the reference to that line
 * if failure return null
 */
char* getReply(int socket, char* suffix, int bytype);

/*
 * method to find the suffix string from the given string
 * if success return the reference to the suffix
 * if failure return null
 */
char* getSuffix(char* line);


#endif /* SMTP_H_ */
