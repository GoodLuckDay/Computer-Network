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
};//소켓을 이용을 하여 입력을 받은 계산식을 구조체에 저장을 하여 전송을 하기위해 구조체 선언
typedef struct cal_data Cal_data;


int main(int argc, char **argv){
	int sockfd, portno, n;//소켓을 저장을 하는 변수와 port번호를 저장하는 변수, 전송과 수신의 결과를 저장을 하는 변수 선언
	int serverlen;//serveraddr의 크기가 저장이되어지는 변수
	struct sockaddr_in serveraddr;//서버의 주소 정버를 저장을 하는 변수
	struct hostent *server;//hostent타입의 구조체가 저장이 되어지는 변수
	char *hostname;//hostname이 저장이 되는 함수
	char buf[BUFSIZE];//계산식을 입력을 하기위해서 char 배열 선언
	int left_num,right_num, result;//입력된 계산식의 연산자와 피연산자를 저장을 하기 위한 변수 선언
	char op;
	Cal_data sdata;//소켓을 이용을하여 전송을 할 구조체 선언
	
	if(argc != 3){//ip주소와 port번호를 입력을 하지 않으면 프로그램을 종료
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
		exit(0);
	}
	hostname = argv[1];//hostname을 저장
	portno = atoi(argv[2]);//port번호를 저장
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);//socket을 생성
	if(sockfd < 0 )//소켓이 생성이 되지 않았다면 에러 출력
		error("ERROR opening socket\n");
	
	server = gethostbyname(hostname);//호스트 name에 상응하는 hostent타입의 구조체를 얻는다.
	if(server == NULL){//hostent의 구조체를 얻지 못하였다면 error출력
		fprintf(stderr,"ERROR, no such host as %s\n", hostname);
		exit(0);
	}
	
	bzero((char *) &serveraddr, sizeof(serveraddr));//버퍼 초기화
	serveraddr.sin_family = AF_INET;//IPv4 인터넷 프로토콜
	bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);//상대의 ip주소를 sesrveraddr.sin_addr.s_addr에 저장
	serveraddr.sin_port = htons(portno);//사용할 port번호 설정
	while(1){
	bzero(buf,BUFSIZE);//계산식을 입력을 받을 char배열 초기화
	printf("> ");//계산식 입력 안내 출력
	fgets(buf, BUFSIZE, stdin);//계산식 입력
	sscanf(buf, "%d%[^0-9]%d", &left_num, &op, &right_num);//입력이 되어진 계산식의 연산자와 피연산자를 각각에 해당하는 변수에 할당
	memset((void *)&sdata, 0x00, sizeof(sdata));//구조체를 초기화:
	sdata.left_num = htonl(left_num);//구조체의 연산자와 피연산자를 나타내는 변수에 값을 초기화
	sdata.right_num = htonl(right_num);
	sdata.op = op;
	serverlen = sizeof(serveraddr);//serveraddr의 크기를 serverlen에 저장
	n = sendto(sockfd, (char *)&sdata, sizeof(sdata), 0, &serveraddr, serverlen);//서버쪽으로 구조체를 전송
	if(n < 0)//전송 실패시 에러 출력
		error("ERROR in sendto\n");
	n = recvfrom(sockfd, (char*)&sdata, sizeof(sdata), 0, &serveraddr, &serverlen);//서버쪽에서 오는 구조체 수신
	if(n < 0)//수신 실패시 에러 출력
		error("ERROR in recvfrom\n");
	result = htonl(sdata.result);//계산의 결과값으로 온값의 byteorder를 변경
	printf("%d %c %d = %d\n",left_num, op, right_num, result);//계산식의 결과를 출력
	}
	return 0;
}
