#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
struct cal_data
{
	int left_num;
	int right_num;
	char op;
	int result;
	short int error;
};//client에서 보내는 구조체와 동일한 형식의 구조체 정의

typedef struct cal_data Cal_data;

void error(char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char **argv){
	Cal_data sdata;//구조체를 선언
	int left_num, right_num, result;//구조체의 값들을 임시로 저장을 하는 변수
	int sockfd;//생성이 되어진 소켓이 저장이 되는 변수
	int portno;//port번호가 저장이 되어지는 변수
	int clientlen;//clientaddr의 크기가 저장이 되어지는 변수
	struct sockaddr_in serveraddr;//서버의 주소정보를 저장하는 변수
	struct sockaddr_in clientaddr;//client의 주소정보를 저장하는 변수
	struct hostent *hostp;//hostent 타입의 구조체가 저장이 되어지는 변수
	char *hostaddrp;
	int optval;
	int n;

	if(argc != 2){//입력된 인자의 개수를 확인을 하고 인자의 개수가 맞지 않으면 오류 출력
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	portno = atoi(argv[1]);//port번호 저장
	sockfd =socket(AF_INET, SOCK_DGRAM, 0);//소켓을 생성 AF_INET은 IPv4의 인터넷프로토콜 SOCK_DGRAM은 udp통신 SOCK_STREAM은 tcp통신
	if(sockfd < 0)//소켓 생성 실패시 오류 메세지 출력
		error("ERROR opening socket");

	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

	bzero((char *) &serveraddr, sizeof(serveraddr));//serveraddr변수 초기화
	serveraddr.sin_family = AF_INET;//IPv4인터넷 프로토콜 사용
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//32bit IPV4주소
	serveraddr.sin_port = htons((unsigned short)portno);//사용할 port번호 초기화

	if(bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)//소켓을 커널에 등록
		error("ERROR on binding");//등록 실패시 에러메세지 출력
	clientlen = sizeof(clientaddr);//clientaddr구조체의 크기 저장
	printf("Start to run server!\n");
	while(1) {
		bzero(&sdata,sizeof(sdata));//sdata 초기화
		n = recvfrom(sockfd, (char *)&sdata, sizeof(sdata), 0, (struct sockaddr *) &clientaddr, &clientlen);//client로 부터 오는 구조체를 sdata에 저장을 한다.
		if(n < 0)//수신 실패시 에러메세지 출력
			error("ERROR in recvfrom");
		left_num = htonl(sdata.left_num);//구조체에 저장이 되어진 정보의 byteorder를 변경 후 변수에 저장
		right_num = htonl(sdata.right_num);
		
		switch(sdata.op)//switch문을 사용하여 해당 연산자에 대하여 계산결과를 저장
		{
			case '+':
				result = left_num + right_num;
				break;
			case '-':
				result = left_num - right_num;
				break;
			case '*':
				result = left_num * right_num;
				break;
			case '/':
				if(right_num == 0)
				{	
					sdata.error = htons(2);
					break;
				}
				result = left_num / right_num;
				break;
		}
		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);//client의 정보를 가지는 hostent구조체 저장
		if(hostp == NULL)//구조체 반환 실패시 에러 메세지 출력
			error("ERROR on inet_ntoa\n");
		hostaddrp = inet_ntoa(clientaddr.sin_addr);//client의 네트워크 바이트 순서의 32비트 값을 Dotted-Decimal Notation의 주소값으로 변경
		if (hostaddrp == NULL)//주소정보 저장 실패시 에러 출력
			error("ERROR on inet_ntoa\n");
		printf("Client Info : %s (%d)\n", hostaddrp, portno);	//client의 주소와 사용중인 port의 번호를 출력
		printf("Input : %d %c %d\n", left_num, sdata.op,right_num);//수신이 되어진 구조체로부터 얻은 피연산자, 연산자들을 출력
		printf("Result : %d\n", result);//연산의 결과를 출력
		sdata.result = htonl(result);//결과값의 byteorder를 변경을 한 후 구조체에 저장
		n = sendto(sockfd, (char *)&sdata, sizeof(sdata), 0, (struct sockaddr *) &clientaddr, clientlen);//client에 결과값을 전송
	
		if( n < 0)//전송 실패시 에러 메세지 출력
			error("ERROR in sendto");
	}
}
