/** Port Scanner
	by: Donald Dimailig
	
	Reference:
		http://www.binarytides.com/tcp-connect-port-scanner-c-code-linux-sockets/
*/

#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

struct hostent *host;
int i, k;
int portstart, portend;
int ipstart, ipend;
char hostname[64];
char octet4[3];

struct sockaddr_in sockaddr;

struct thread_args {
	int portno;
	char host_name[64];
};

char *itobase10(char *buf, int value) {
    sprintf(buf, "%d", value);
    return buf;
}

void *connectThread(void *argp) {
	struct thread_args *args = argp;
	char hname[64];
	int pno = args->portno;
	int err, sock;
	
	strcpy(hname, args->host_name);
	
	sockaddr.sin_port = htons(pno);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sock < 0) {
		perror("\nSocket");
		exit(1);
	}
	// Connect using created socket
	err = connect(sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
	
	if(err < 0) {
		fflush(stdout);
	// connected port is open
	} else {
		printf("%-5d\topen\t%s\n", pno, hname);
	}
	close(sock);
}

int main (int argc, char *argv[]) {
	
	char argv1[100], argv2[100], iptemp[100], octet1to3[100];
	char *token;
	const char delim1[2] = "-";
	const char delim2[2] = "/";
	int m, n, o, temp;
	
	pthread_t tid;
	
	// Initialization of the sockaddr_in struct
	strncpy ((char*)&sockaddr, "", sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	
	if(argv[1] == NULL) {
		printf("##################################\n");
		printf("# How To Use:\t\t\t #\n");
		printf("# A. Default port range (1-1000) #\n");
		printf("#    ./x hostname\t\t #\n");
		printf("#    ./x IP address\t\t #\n");
		printf("#    ./x IP range\t\t #\n");
		printf("#    ./x network/24\t\t #\n");
		printf("# B. Specific port\t\t #\n");
		printf("#    ./x hostname port\t\t #\n");
		printf("#    ./x IP address port\t #\n");
		printf("#    ./x IP range port\t\t #\n");
		printf("#    ./x network/24 port\t #\n");
		printf("# C. Port range\t\t\t #\n");
		printf("#    ./x hostname portrange\t #\n");
		printf("#    ./x IP address portrange\t #\n");
		printf("#    ./x IP range portrange\t #\n");
		printf("#    ./x network/24 portrange\t #\n");
		printf("##################################\n");
		exit(3);
	} else {
		strcpy(hostname, argv[1]);
		if(isdigit(hostname[0])) {
			strcpy(argv1, argv[1]);
			strcpy(iptemp, argv[1]);
			token = strtok(argv1, ".");
			token = strtok(NULL, ".");
			token = strtok(NULL, ".");
			token = strtok(NULL, "-");
			ipstart = atoi(token);
			if(strchr(iptemp, '-') == NULL && strchr(iptemp, '/') == NULL){
				ipend = ipstart;
			} else {
				if(strchr(iptemp, '-') != NULL) {
					token = strtok(NULL, "-");
					ipend = atoi(token);
				}
				if(strchr(iptemp, '/') != NULL) {
					ipstart = 1;
					ipend = 255;
				}
			}
		}
	}
	
	if(argv[2] == NULL) {
		portstart = 1;
		portend = 10000;
	} else {
		strcpy(argv2, argv[2]);
		if(strchr(argv2, '-') != NULL){
			token = strtok(argv2, "-");
			portstart = atoi(token);
			token = strtok(NULL, "-");
			portend = atoi(token);
		} else {
			portstart = atoi(argv2);
			portend = atoi(argv2);
		}
	}
	
	printf("Port\tStatus\tHostname/IP\n");
	
	// If IP address is used
	if(isdigit(hostname[0])) {
		memset(iptemp, 0, sizeof(iptemp));
		token = strtok(hostname, ".");
		strcat(iptemp, token);
		strcat(iptemp, ".");
		token = strtok(NULL, ".");
		strcat(iptemp, token);
		strcat(iptemp, ".");
		token = strtok(NULL, ".");
		strcat(iptemp, token);
		strcat(iptemp, ".");
		strcpy(octet1to3, iptemp);
		
		for(k = ipstart; k <= ipend; k++) {
			memset(octet4, 0, sizeof(octet4));
			memset(iptemp, 0, sizeof(iptemp));
			strcpy(iptemp, octet1to3);
			itobase10(octet4, k);
			strcat(iptemp, octet4);
			sockaddr.sin_addr.s_addr = inet_addr(iptemp);
			
			// Port scan loop
			for(i = portstart; i <= portend; i++) {
				struct thread_args *args = malloc(sizeof (*args));
				args->portno = i;
				strcpy(args->host_name, iptemp);
				pthread_create(&tid, NULL, connectThread, args);
				usleep(100);
			}
		}
	// If Hostname is used
	} else if((host = gethostbyname(hostname)) != 0) {
		strncpy ((char*)&sockaddr.sin_addr, (char*)host->h_addr, sizeof(sockaddr.sin_addr));
		for(i = portstart; i <= portend; i++) {
				struct thread_args *args = malloc(sizeof (*args));
				args->portno = i;
				strcpy(args->host_name, hostname);
				pthread_create(&tid, NULL, connectThread, args);
				usleep(100);
		}
	} else {
		herror(token);
		exit(2);
	}
	
	sleep(1);
	
	return 0;
	
}
