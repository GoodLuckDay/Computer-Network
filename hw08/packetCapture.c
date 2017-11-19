#include <netinet/in.h>
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PACKET_LENGTH 65536
void PacketCapture(unsigned char *buf, int size);
void tcp_packet(unsigned char *buf, int size);
void icmp_packet(unsigned char *buf, int size);
void udp_packet(unsigned char *buf, int size);
void printEtherHeader(unsigned char *buf);
void printIpHeader(struct iphdr *iph);
int main(int argc, char **argv){
  int readn;
  socklen_t addrlen;
  int sock_raw;
  struct sockaddr_in saddr;

  unsigned char *buffer = (unsigned char *)malloc(PACKET_LENGTH);//패킷의 크기 만큼 메모리를 할당
  sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));//raw소켓을 TCP형식으로 생성
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
    PacketCapture(buffer, readn);//수신 되어진 패킷에 대하여 출력
  }
  close(sock_raw);
  return 0;
}

void PacketCapture(unsigned char* buffer, int size){
  struct iphdr *iph = (struct iphdr*)(buffer+ETHER_HDR_LEN);
  switch (iph->protocol) {
    case 1:
      printEtherHeader(buffer);
      printIpHeader(iph);
      icmp_packet(buffer+ETHER_HDR_LEN+iph->ihl*4, size);
      break;
    case 6:
      printEtherHeader(buffer);
      printIpHeader(iph);
      tcp_packet(buffer+ETHER_HDR_LEN+iph->ihl*4, size);
      break;
    case 17:
      printEtherHeader(buffer);
      printIpHeader(iph);
      udp_packet(buffer+ETHER_HDR_LEN+iph->ihl*4, size);
      break;
    default:
      break;
  }
}

void printEtherHeader(unsigned char *buf){
  printf("Ethernet Header\n");
  struct ether_header *etherh = (struct ether_header *)buf;
  printf("\t|-Destination MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);
  printf("\t|-Source MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
  printf("\t|-Protocol : %u\n", etherh->ether_type);
}
void printIpHeader(struct iphdr *iph){
  printf("IP Header\n");
  printf("\t|-IP Versiont :%u\n", iph->version);
  printf("\t|-IP Header Length :%u DWORDS or %u Bytes\n", iph->ihl, iph->ihl*4);
  printf("\t|-Type Of Service :%u\n", iph->tos);
  printf("\t|-IP Total Length :%u\n", iph->tot_len);
  printf("\t|-Identification :%u\n", iph->id);
  printf("\t|-TTL :%u\n", iph->ttl);
  printf("\t|-Protocol :%u\n", iph->protocol);
  printf("\t|-Checksum :%u\n", iph->check);
  printf("\t|-Source IP : %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
  printf("\t|-Destination IP : %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
}

void icmp_packet(unsigned char *buf, int size){
  struct icmphdr *icmph = (struct icmphdr *)buf;
  printf("ICMP Header\n");
  printf("\t|-Type : %u\t|-Code : %u\n", icmph->type, icmph->code);
  printf("\t|-Checksum : %u\n", icmph->checksum);
}
void udp_packet(unsigned char *buf, int size){
  struct udphdr *udph = (struct udphdr *)buf;
  printf("UDP Header\n");
  printf("\t|-Source Port : %u\n",udph->source);
  printf("\t|-Destination Port : %u\n",udph->dest);
  printf("\t|-UDP Length : %u\n",udph->len);
  printf("\t|-UDP Checksum : %u\n",udph->check);
}

void tcp_packet(unsigned char* buf, int size){
  struct tcphdr *tcph = (struct tcphdr*)buf;
  printf("TCP Header\n");
  printf("\t|-Source Port : %u\n",tcph->source);
  printf("\t|-Destination Port : %u\n",tcph->dest);
  printf("\t|-Sequence Number : %u\n",tcph->seq);
  printf("\t|-Acknowledge Number : %u\n",tcph->ack_seq);
  printf("\t|-Header Length : %u DWORDS or %u BYTES\n",tcph->doff, (tcph->doff) * 4);
  printf("\t|-Urgent Flag : %u\n",tcph->urg);
  printf("\t|-Acknowledge Flag : %u\n",tcph->ack);
  printf("\t|-Push Flag : %u\n",tcph->psh);
  printf("\t|-Reset Flag : %u\n",tcph->rst);
  printf("\t|-Synchronise Flag : %u\n",tcph->syn);
  printf("\t|-Finish Flag : %u\n",tcph->fin);
  printf("\t|-Window : %u\n",tcph->window);
  printf("\t|-Checksum : %u\n",tcph->check);
  printf("\t|-UIrgent Pointer : %u\n",tcph->urg_ptr);

}
