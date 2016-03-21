/*
 * clien.c
 * simple mail transfer protocol
 *
 *  Created on: Dec 20, 2014
 *      Author: Gili Mizrahi  302976840
 */

#include "SMTP.h"
#include <errno.h>

int main(int argc, char** argv){


	int client, portNum;
	FILE* file;


	if(argc < 3 || argc > 4){
		printf("please enter follow: %s path_to_file SMTP_server port(optional)\n", argv[0]);
		return 1;
	}

	else if(argc == 4)
		portNum = atoi(argv[3]);

	else
		portNum = 25;


	if((client = createSocket(argv[2], portNum)) < 0){
		printf("%s\n", strerror(errno));
		return 1;
	}

	if((file = fopen(argv[1], "r")) == NULL){
		printf("%s\n", strerror(errno));
		return 1;
	}

	if((smtp(file, client)) == 1){
		close(client);
		fclose(file);
		printf("%s\n", strerror(errno));
		return 1;
	}

	close(client);
	fclose(file);

	return 0;
}
