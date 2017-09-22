#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024//버퍼의 최대 크기를 나타낸다
#define OPSZ 4//피연산자의 크기를 나타낸다.

void error_handling(char* message);//매개변수로온 문장을 에러 메세지로 출력을 하는 함수이다.
int calculate(int opnum, int opnd[], char operator);//client에서 온 연산자와 피연산자들을 계산을 하여 반환을 한다.

int main(int argc, char* argv[]) {

	int serv_sock, clnt_sock;//생성된 server의 소켓을 저장을 하는 변수와, 받아진 client의 socket을 저장하는 변수
	struct sockaddr_in serv_addr, clnt_addr;//각각 Server와 Client의 정보를 저장을 한다.
	socklen_t clnt_adr_sz;//clnt_addr의 크기가 저장이 되는 변수
	char opinfo[BUF_SIZE];
	int result, opnd_cnt, i;//
	int recv_cnt, recv_len;//

	if(argc !=2) {//인자의 개수가 맞지 않다면 에러를 출력한다.
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);//IPv4의 TCP소켓 생성
	if(serv_sock == -1)//소켓 생성 실패시 에러메세지 출력
		error_handling("socker() error");
	memset(&serv_addr, 0, sizeof(serv_addr));//serv_addr 구조체 초기화
	serv_addr.sin_family=AF_INET;//IPv4 인터넷 프로토콜 사용
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//32bit IPV4주소
	serv_addr.sin_port=htons(atoi(argv[1]));//사용할 port번호 초기화

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) //소켓을 커널에 등록
		error_handling("bind() error");//등록 실패시 오류 메세지 출

	if(listen(serv_sock, 5) == -1) //수신 대기열 5개를 생성
		error_handling("listen() error");//생성 실패시 오류 메세지 출력


	clnt_adr_sz = sizeof(clnt_addr);//clnt_addr 구조체의 크기를 clnt_adr_sz에 저장

	for(i=0; i<5; i++) {
		opnd_cnt = 0;
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_sz);//클라이언트 접속 요청 대기 및 허락, 클라이언트와 통신을 위해 새 socket생성
		read(clnt_sock, &opnd_cnt, 1);//소켓에서 1바이트 만큼 읽고 opnd_cnt에 저장을 한다.

		recv_len = 0;
		while((opnd_cnt*OPSZ+1) >recv_len) {//피연산자의 개수만큼 byte를 읽으면 반복문을 종료한다.
			recv_cnt=read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);
			recv_len+=recv_cnt;//읽은 Byte를 recv_len에 더해준다.
		}

		result = calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);//Calculate함수의 반환값을 result변수에 저장한다.
		write(clnt_sock, (char*)&result, sizeof(result));//소켓에 result를 저장한다.
		close(clnt_sock);//client 소켓을 종료한다.
	}
	close(serv_sock);//server 소켓을 종료한다.
	return 0;

	
}

int calculate(int opnum, int opnds[], char op) {
	int result = opnds[0], i;//첫번째 피연산자를 result변수에 저장하고, 반복문을 위해서 변수 i 선언
	switch(op) {//Switch문을 이용하여 연산자의 종류에 다른 연산을 수행한다.
		case '+':
				for(i=1; i<opnum; i++) result +=opnds[i];//두번째 피연산자 부터 마지막 피연산자까지 모두 더해 모든 피연산자를 더한 값을 result변수에 저장
				break;
		case '-':
				for(i=1; i<opnum; i++) result -=opnds[i];//두번째 피연산자 부터 마지막 피연산자까지 모두 빼 모든 피연산자를 뺀 값을 result변수에 저장
				break;
		case '*':
				for(i=1; i<opnum; i++) result *=opnds[i];//두번째 피연산자 부터 마지막 피연산자까지 모두 곱해 모든 피연산자를 곱한 값을 result변수에 저장
				break;
	}
	return result;//계산된 결과값을 반환한다.

}

void error_handling(char *message) {//에러메세지를 출력을 하는 함수이다.
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


