#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024//버퍼의 최대 크기를 나타내는 상수
#define RLT_SIZE 4
#define OPSZ 4//피연산자의 바이트 크기를 나타내는 상수

void error_handling(char* message);

int main(int argc, char* argv[]) {

	int sock;//생성이 되어진 소켓이 저장이 되어진 변수
	struct sockaddr_in serv_addr;//서버에 대한 정보가 저장이 되는 구조체
	char opmsg[BUF_SIZE];//
	int result, opnd_cnt, i;//계산의 결과, 피연산자의 수, 반복문을 위한 변수의 선언

	if(argc !=3) {//Port번호와 IP를 입력하지 않으면 에러를 출력한다.
		printf("Usage : %s  <IP> <port> \n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);//IPv4의 TCP 소켓을 생성
	if(sock == -1)//소켓 생성 실패시 오류 메세지 출력
		error_handling("socker() error");

	memset(&serv_addr, 0, sizeof(serv_addr));//serv_addr 구조체 초기화
	serv_addr.sin_family=AF_INET;//IPv4 인터넷 프로토콜 사용
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);//서버의 주소 저장
	serv_addr.sin_port=htons(atoi(argv[2]));//Port번호 저장

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)//Connect함수를 이용하여 서버와 연결한다.
		error_handling("connect() error");//연결 실패 시 에러메세지 출력

	fputs("oper count : ", stdout);//피연산자의 개수를 입력받는다.
	scanf("%d", &opnd_cnt);
	opmsg[0] = (char)opnd_cnt;//피연산자의 개수를 server에 보낼 Buffer에 저장한다.


	for(i=0; i<opnd_cnt; i++) {//피연산자를 입력받는다.
		printf("operand %d : ", i+1);
		scanf("%d", (int*)&opmsg[i*OPSZ+1]);
	}
	fgetc(stdin);
	fputs("operator : ", stdout);//연산자를 입력받는다.
	scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);//연산자를 입력받은 피연산자 다음에 byte에 저장한다.
	write(sock, opmsg, opnd_cnt*OPSZ+2);//서버에 소켓을 전송한다.
	read(sock, &result, RLT_SIZE);//result변수에 서버로부터 온 결과 값을 저장한다.

	printf("result = %d\n\n", result);//result에 저장된 결과값을 출력한다.

	close(sock);//소켓을 종료한다.
	return 0;
}


void error_handling(char *message) {//매개변수로 온 message에 저장이 된 문장을 에러메세지로 출력을 해준다.
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


