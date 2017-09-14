#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

void error(char *msg) {
	perror(msg);
	exit(0);
}

struct cal_data
{
	int left_num;
	int right_num;
	char op;
	int result;
	short int error;
};
typedef struct cal_data Cal_data;


int main(int argc, char **argv){
	int sockfd, portno, n;
	int serverlen;
	struct sockaddr_in serveraddr;
	struct hostent *server;
	char *hostname;
	char buf[BUFSIZE];
	int left_num,right_num, result;
	char op[1];
	Cal_data sdata;
	
	if(argc != 3){
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
		exit(0);
	}
	hostname = argv[1];
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0 )
		error("ERROR opening socket\n");
	
	server = gethostbyname(hostname);
	if(server == NULL){
		fprintf(stderr,"ERROR, no such host as %s\n", hostname);
		exit(0);
	}
	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);
	while(1){
	bzero(buf,BUFSIZE);
	printf("> ");
	fgets(buf, BUFSIZE, stdin);
	sscanf(buf, "%d%[^0-9]%d", &left_num, op, &right_num);
	memset((void *)&sdata, 0x00, sizeof(sdata));
	sdata.left_num = htonl(left_num);
	sdata.right_num = htonl(right_num);
	sdata.op = op[0];
	serverlen = sizeof(serveraddr);
	n = sendto(sockfd, (char *)&sdata, sizeof(sdata), 0, &serveraddr, serverlen);
	if(n < 0)
		error("ERROR in sendto\n");
	n = recvfrom(sockfd, (char*)&sdata, sizeof(sdata), 0, &serveraddr, &serverlen);
	if(n < 0)
		error("ERROR in recvfrom\n");
	result = htonl(sdata.result);
	printf("%d %c %d = %d\n",left_num, op[0], right_num, result);
	}
	return 0;
}
