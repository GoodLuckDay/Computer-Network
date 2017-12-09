#include <netinet/in.h>
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PACKET_LENGTH 65536
void PrintPacket(unsigned char *buf, int size);
void PrintTcp(unsigned char *buf, int size);

int main(int argc, char **argv){
  int readn;
  socklen_t addrlen;
  int sock_raw;
  struct sockaddr_in saddr;

  unsigned char *buffer = (unsigned char *)malloc(PACKET_LENGTH);//패킷의 크기 만큼 메모리를 할당
  sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);//raw소켓을 TCP형식으로 생성
  if(sock_raw < 0){//소켓 생성 실패시 에러 출력
    return 1;
  }
  while(1){
    addrlen = sizeof(saddr);
    memset(buffer, 0x00, PACKET_LENGTH);
    readn = recvfrom(sock_raw, buffer, PACKET_LENGTH, 0, (struct sockaddr *)&saddr, &addrlen);//패킷을 수신 받는다.
    if(readn < 0){//패킷 수신 실패시 에러메세지
      return 1;
    }
  
    PrintPacket(buffer, readn);//수신 되어진 패킷에 대하여 출력
  }
  close(sock_raw);
  return 0;
}

void PrintPacket(unsigned char* buffer, int size){
  struct iphdr *iph = (struct iphdr*)buffer;
  printf("protocol : %d\n", iph->protocol);//현재 온 packet의 protocol값을 출력한다.
  switch (iph->protocol) {
    case 6:
      PrintTcp(buffer, size);//패킷이 TCP일때 출력
      break;
    default:
      break;
  }
}

void PrintTcp(unsigned char* buf, int size){
  unsigned short iphdrlen;
  unsigned char *data;
  struct iphdr *iph = (struct iphdr *)buf;
  iphdrlen = iph->ihl*4;//header의 크기를 저장
  struct tcphdr *tcph = (struct tcphdr*)(buf + iphdrlen);//패킷의 시작 주소를 저장

  data = (unsigned char *)(buf + (iph->ihl*4) + (tcph->doff*4));//패킷에 데이터값을 저장
  printf("%s", data);
}
