import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;

public class NAT_Homework {
    static BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
    static final int[] myaddress = {192, 168, 0, 5};//사용자의 IP주소
    static final int myport = 6712;//사용자의 port번호
    static final int[] routeraddress = {168,188,123,60};//라우터의 IP주소
    static int entry = 0;
    public static void main(String[] args) throws IOException {//IP와 Port번호를 입력을 받고 NAT를 구현
        Packet mypacket = new Packet();
        NAT[] table = new NAT[10];//Table생성

        String str = "Y";
        while(true){
            sender_packet(mypacket);//사용자가 전송을 하는 packet을 생성
            router_private(table, mypacket);//rounter가 private 네트워크에서 packet을 받아 universal 네트워크로 packet을 전송
            receiver_packet(mypacket);//수신자가 packet을 받은 후 라우터의 IP를 destination으로 설정하여 packet을 전송
            router_public(table, mypacket);//수신자가 보낸 packet을 받은 라우터는 테이블을 검색하여 알맞은 private 네트워크의 호스트에게 packet을 전송한다.
            System.out.println(entry);
            System.out.println("Continue (Y/N)");//패킷을 그만 보낼려면 'N' 또는 'n'문자를 키보드에 입력한다.
            str = bufferedReader.readLine();
            if(str.equals("N") || str.equals("n")){
                break;
            }
        }
    }

    private static void router_public(NAT[] table, Packet mypacket) {//universal 네트워크에서 private 네트워크로 패킷을 전송을 하는 과정이다.
        System.out.println("NAT Receives Packet from External Network\n");
        print_packet(mypacket);
        for(int i=0; i<(entry >= 10 ? 10 : entry); i++){//테이블에서 해당하는 entry를 검색한다.
            if((Arrays.equals(mypacket.sip,table[i].extip) && mypacket.sport == table[i].extport) && (mypacket.dport == table[i].priport)){//해당하는 entry를 찾으면 dip를 해당 entry의 priip값으로 수정을 해준다.
                mypacket.dip = Arrays.copyOf(table[i].priip, table[i].priip.length);
                System.out.println("NAT Sends Packets to Private Network\n");
                print_packet(mypacket);
            }
        }

    }
    private static void receiver_packet(Packet mypacket) {//수신자가 packet을 받으면 sip와 dip값을 서로 뒤바꾼고 sport , dport 값들 또한 서로 뒤 바꾸어 준다.
        System.out.println("Receiver Receives Packet\n");
        print_packet(mypacket);
        swapIP(mypacket);
        swapPort(mypacket);
        System.out.println("Receiver Sends Packet\n");
        print_packet(mypacket);
    }

    private static void swapIP(Packet mypacket) {//패킷 객체에 저장된 sip와 dip를 서로 뒤바꾼다.
        int[] tempIP = Arrays.copyOf(mypacket.sip, mypacket.sip.length);
        mypacket.sip = Arrays.copyOf(mypacket.dip, mypacket.dip.length);
        mypacket.dip = Arrays.copyOf(tempIP, tempIP.length);
    }

    private static void swapPort(Packet mypacket) {//패킷 객체에 저장된 sport와 dport에 저장된 값을 서로 뒤 바꾼다.
        int tempPort = mypacket.sport;
        mypacket.sport = mypacket.dport;
        mypacket.dport = tempPort;
    }

    private static void router_private(NAT[] table, Packet mypacket) {
        boolean found = false;
        System.out.println("NAT Receives Packet from Private Networks\n");
        print_packet(mypacket);
        System.out.println("New Entry");

        for(int i=0; i<(entry >= 10 ? 10 : entry); i++){//entry가 10개를 넘었다면 테이블이 가득 찬 상황임으로 모든 Entry들을 살펴보아야 한다. 하지만 10개를 넘지 않는 다면 entry의 개수 만큼만 살펴보면 된다.
            if((Arrays.equals(table[i].priip, mypacket.sip) && table[i].priport == mypacket.sport) && (Arrays.equals(table[i].extip, mypacket.dip) && table[i].extport == mypacket.dport)){
                found = true;//entry가 이미 테이블에 존재한다면 entry를 생성을 하지 않는다.
            }
        }
        if(!found) {
            table[entry%table.length] = new NAT();//entry가 존재하지 않는다면 새로운 entry를 생성을 하며. 만약 테이블이 꽉찬 상황이라면 테이블의 처음 부터 다시 덮어쓰기를 한다.
            table[entry%table.length].priip = Arrays.copyOf(mypacket.sip, mypacket.sip.length);
            table[entry%table.length].priport = mypacket.sport;
            table[entry%table.length].extip = Arrays.copyOf(mypacket.dip, mypacket.dip.length);
            table[entry%table.length].extport = mypacket.dport;
            entry = entry + 1;
        }
        mypacket.sip = Arrays.copyOf(routeraddress, routeraddress.length);//packet의 sip 값을 라우터의 IP값으로 변경 시켜준다.
        print_tableEntry(table);
        System.out.println("NAT Sends Packet to External Network\n");
        print_packet(mypacket);
    }

    private static void print_tableEntry(NAT[] table) {//현재 NAT Table에 저장된 Entry들을 모두 출력한다.
        System.out.println("====Current NAT Table Entry====\n");
        for(int i=0; i<(entry >= 10 ? 10 : entry); i++){//entry가 10개를 넘었다면 테이블이 가득 찬 상황임으로 모든 Entry들을 살펴보아야 한다. 하지만 10개를 넘지 않는 다면 entry의 개수 만큼만 살펴보면 된다.
            System.out.println(table[i].priip[0]+"."+table[i].priip[1]+"."+table[i].priip[2]+"."+table[i].priip[3]+" "+table[i].priport+" "+table[i].extip[0]+"."+table[i].extip[1]+"."+table[i].extip[2]+"."+table[i].extip[3]+" "+table[i].extport);
        }
        System.out.println();
    }

    private static void sender_packet(Packet mypacket) throws IOException {//IP와 Port번호를 입력을 받고 packet에 저장을 한다.
        mypacket.sip = Arrays.copyOf(myaddress, myaddress.length);//사용자의 IP와 port번호 저장
        mypacket.sport = myport;
        System.out.println("Enter destination address");
        String[] destIP = bufferedReader.readLine().split(" ");//상대방의 IP와 port번호를 입력을 받고 Packet객체에 저장
        mypacket.dip[0] = Integer.parseInt(destIP[0]);
        mypacket.dip[1] = Integer.parseInt(destIP[1]);
        mypacket.dip[2] = Integer.parseInt(destIP[2]);
        mypacket.dip[3] = Integer.parseInt(destIP[3]);
        System.out.println("Enter destination port");
        mypacket.dport = Integer.parseInt(bufferedReader.readLine());

        System.out.println("Sender Sends");
        print_packet(mypacket);
    }

    private static void print_packet(Packet mypacket) {//매개변수로 온 Packet객체에 담긴 정보들을 출력을 하는 메소드이다.
        System.out.println("====Packet information====");
        System.out.println("Source IP : "+mypacket.sip[0]+", "+mypacket.sip[1]+", "+mypacket.sip[2]+", "+mypacket.sip[3]);
        System.out.println("Dest   IP : "+mypacket.dip[0]+", "+mypacket.dip[1]+", "+mypacket.dip[2]+", "+mypacket.dip[3]);
        System.out.println("Source PORT : "+mypacket.sport);
        System.out.println("Dest   Port : "+mypacket.dport);
        System.out.println("===============================\n");
    }

    static class Packet{//패킷을 구현하기 위해서 만든 클래스이다.
        int[] sip = new int[4];
        int[] dip = new int[4];
        int sport;
        int dport;
    }

    static class NAT{//NAT 테이블을 구현하기 위해서 만든 클래스
        int[] priip = new int[4];
        int priport;
        int[] extip = new int[4];
        int extport;
    }
}


