//
// Created by haon4658 on 17. 11. 9.
//
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

int in_cksum(u_short *p, int n);//checksum을 계산을 하기 위한 함수

int main(int argc, char **argv){
    int icmp_socket;
    int ret;
    struct icmp *p, *rp;
    struct sockaddr_in addr, from;
    struct ip *ip;
    char buffer[1024];
    socklen_t sl;
    int hlen;

    icmp_socket = socket(AF_INET, SOCK_RAW, 1);//RAW소켓을 생성 protocol은 ICMP로 설정
    if(icmp_socket < 0 ){//소켓 생성 실패시 에러
        perror("socket error : ");
        exit(0);
    }

    memset(buffer, 0x00, 1024);//버퍼 초기화

    p = (struct icmp *)buffer;
    p->icmp_type = ICMP_ECHO;
    p->icmp_cksum = 0;
    p->icmp_seq = 15;
    p->icmp_id = getpid();

    p->icmp_cksum = in_cksum((u_short *)p, 1000);
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);//프로그램을 실행하였을 때의 같이 입력한 IP조수를 세팅
    addr.sin_family = AF_INET;

    ret = sendto(icmp_socket, p, sizeof(*p), MSG_DONTWAIT, (
    struct sockaddr *)&addr, sizeof(addr));//소켓을 전송

    if(ret < 0){
        perror("sendto error : ");//전송 실패시 에러
    }

    sl = sizeof(from);
    ret = recvfrom(icmp_socket, buffer, 1024, 0, (//소켓을 수신
    struct sockaddr *)&from, &sl);

    if(ret < 0 ){//수신 실패시 에러 메세지 출력
        printf("%d %d %d\n", ret, errno, EAGAIN);
        perror("recvfrom error : ");
    }

    ip = (struct ip*)buffer;//전송 받은 값을 저장
    hlen = ip->ip_hl*4;//IP Header의 *4를 해줌으로 실제 헤더의 크기를 얻는다.
    rp = (struct icmp *)(buffer+hlen);//소켓의 시작 위치를 저장을 한다.
    printf("reply from %s\n", inet_ntoa(from.sin_addr));
    printf("Type : %d \n", rp->icmp_type);
    printf("Code : %d \n", rp->icmp_code);
    printf("Seq : %d \n", rp->icmp_seq);
    printf("Iden : %d\n", rp->icmp_id);
    return 1;
}

int in_cksum(u_short *p, int n){//체크섬을 계산을 해주는 함수
    register u_short answer;
    register long sum = 0;
    u_short odd_byte = 0;
    while(n > 1){
        sum += *p++;
        n -=2;
    }
    if(n == 1){
        *(u_char*)(&odd_byte) = *(u_char*)p;
        sum+=odd_byte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;//더한 값들에 NOT연산을 하여 준다.
    return (answer);
}
