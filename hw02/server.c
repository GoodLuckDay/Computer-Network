#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4

void error_handling(char* message);//매개변수로온 문장을 에러 메세지로 출력을 하는 함수이다.
int calculate(int opnum, int opnd[], char operator);//client에서 온 연산자와 피연산자들을 계산을 하여 반환을 한다.

int main(int argc, char* argv[]) {

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_adr_sz;//client
	char opinfo[BUF_SIZE];
	int result, opnd_cnt, i;
	int recv_cnt, recv_len;

	if(argc !=2) {//인자의 개수가 맞지 않다면 에러를 출력한다.
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socker() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1) 
		error_handling("listne() error");


	clnt_adr_sz = sizeof(clnt_addr);

	for(i=0; i<5; i++) {
		opnd_cnt = 0;
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_sz);
		read(clnt_sock, &opnd_cnt, 1);

		recv_len = 0;
		while((opnd_cnt*OPSZ+1) >recv_len) {
			recv_cnt=read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);
			recv_len+=recv_cnt;
		}

		result = calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);
		write(clnt_sock, (char*)&result, sizeof(result));
		close(clnt_sock);
	}
	close(serv_sock);
	return 0;

	
}

int calculate(int opnum, int opnds[], char op) {
	int result = opnds[0], i;
	switch(op) {
		case '+':
				for(i=1; i<opnum; i++) result +=opnds[i];
				break;
		case '-':
				for(i=1; i<opnum; i++) result -=opnds[i];
				break;
		case '*':
				for(i=1; i<opnum; i++) result *=opnds[i];
				break;
	}
	return result;

}

void error_handling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


