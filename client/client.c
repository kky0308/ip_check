#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<signal.h>

#include<stdint.h>
#include<wiringPi.h>
#include<fcntl.h>
#include<termios.h>
#include<sys/types.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

#define MAXTIMINGS 83
#define DHTPIN 7

void* send_temp(void* arg);//온, 습도의 데이터를 server로 전송하는 함수
void* send_msg(void* arg);//데이터를 server로 전송하는 함수
void* recv_msg(void* arg);//server에서 전송된 데이터를 받는 함수
void Read_dht11();//dht11 sensor의 온, 습도를 받아 server로 전송해주는 함수

char name[NAME_SIZE] = "[DEFAULT]";//
char msg[BUF_SIZE];//메세지를 저장할 배열 선언
int dht11_dat[5] = {0};//dht11 senser의 온, 습도 센서 데이터를 받아 저장하는 배열 선언
int G_sock;//소켓을 전역변수로 전환하는 변수 선언
char temp_buf[25];//dht11에서 받은 데이터를 server로 출력해주는 배열 선언


int main(int argc, char* argv[])
{
   int sock;//Client 와 server를 연결 시킬 소켓을 생성
   struct sockaddr_in serv_addr;//server에 대한 주소를 저장하는 변수 선언
   pthread_t snd_thread, rcv_thread, temp_thread;//쓰레드 변수 선언
   void* thread_return;//pthread_join()에서 쓰레드의 main 함수가 반환하는 값이 저장될 포인터 변수의 주소변수 선언

   if(argc != 4)//입력받은 인수가 3개가 아니라면
   {
      printf("Usage : %s <IP>  <port> <name>\n\n", argv[0]);//실행 문구 출력
      exit(1);//프로그램 종료
   }

   sprintf(name,"[%s]",argv[3]);//마지막에 입력받은 인수를 통하여 Client이름으로 저장
   sock = socket(PF_INET, SOCK_STREAM,0);//Client 소켓 생성

   memset(&serv_addr,0,sizeof(serv_addr));//serv_addr 버퍼 초기화
   serv_addr.sin_family = AF_INET;//IPv4 인터넷 프로토콜
   serv_addr.sin_addr.s_addr = inet_addr(argv[1]);//입력받은 IP주소를 server주소로 대입
   serv_addr.sin_port = htons(atoi(argv[2]));//두번째로 받은 인수를 port로 대입

   if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))== -1)//server와 연결하는데 문제가 생겼다면
   {
      printf("connect() error\n\n");//경고 문구 출력
      exit(1);//프로그램 종료
   }

   pthread_create(&snd_thread,NULL,send_msg,(void*)&sock);//send_msg에 대한 쓰레드 생성
   pthread_create(&rcv_thread,NULL,recv_msg,(void*)&sock);//recv_msg에 대한 쓰레드 생성
   pthread_create(&temp_thread,NULL,send_temp,(void*)&sock);//send_temp 에 대한 쓰레드 생성
   pthread_join(snd_thread, &thread_return);
   pthread_join(rcv_thread, &thread_return);
   pthread_join(temp_thread,&thread_return);//쓰레드 사용도중 프로그램이 종료될 수 없으며 각각의 쓰레드 반환

   close(sock);//소켓에 대한 파일을 닫는다
   return 0;
}
void sigint_handler(int signo)
{
   Read_dht11();//시그널이 발생하였다면 Read_dht11()호출
   alarm(60);//60sec간 딜레이
}
void* send_temp(void* arg)
{
   int sock = *((int*)arg);//인자로 받아온 소켓값을 sock에 저장
   G_sock = sock;//소켓값을 전역변수로 전환
   signal(SIGALRM,sigint_handler);//알람 함수 시그널이 발생하면 sigint_handler함수를 호출한다.
   alarm(1);
}
void* send_msg(void* arg)//serve이r에 데이터를 전송할때 사용되는 함수
{
   int sock = *((int*)arg);//인자로 받아온 소켓값을 sock에 저장

   char name_msg[NAME_SIZE+BUF_SIZE];//이름과 메세지를 통시에 출력하기 위한 배열 선언
   while(1)//무한루프 생성
   {
      fgets(msg,BUF_SIZE,stdin);//키보드 입력을 msg에 저장
      if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))//만약 입력받은 입력이 Q 또는 q 라면
      {
         close(sock);//소켓에 대한 파일을 닫는다.
         exit(1);//프로그램 종료
      }
      sprintf(name_msg,"%s > %s",name,msg);//해당 Client에 대한 닉네임과 키보드입력을 name_msg 배열에 저장
      write(sock, name_msg,strlen(name_msg));//server에 닉네임과 메세지를 데이터로 전송
      memset(msg,0,sizeof(msg));//이전 버퍼를 지우기 위한 명령문

   }
   return NULL;
}

void* recv_msg(void* arg)//server로 부터 데이터를 받을때 사용되는 함수
{
   int  sock = *((int*)arg);//인자로 받아온 소켓값을 sock에 저장
   char name_msg[NAME_SIZE+BUF_SIZE];//server에서 전송된 데이터를 받는 변수 선언
   int str_len;//입력받은 해당 문자열의 길이를 저장하는 변수 선언
   while(1)//무한루프 생성
   {
      str_len=read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);//'\0'를 제외한 크기만큼을 데이터로 받는다
      if(str_len==-1)//만약 read()에서 오류가 생겼다면
      {
         return (void*)-1;//-1을 반환
      }
      name_msg[str_len]=0;//입력받은 데이터 마지막에 '\0' 저장
      fputs(name_msg,stdout);//화면으로 입력받은 데이터를 출력
   }
   return NULL;
}

void Read_dht11()
{
        memset(dht11_dat,0,sizeof(dht11_dat));//배열 초기화

        if(wiringPiSetup() == -1)
        {
                printf("erro ocure!!\n");
                exit(1);
        }
        uint8_t last_state = HIGH;//uint8_t는 unsigned 8bit type을 의미한다.last_state는 이전상태가 Low인지 High
        uint8_t counter = 0;
        uint8_t j =0;
        uint8_t i;

        pinMode(DHTPIN, OUTPUT);//start 시키기 위하여 출력값으로 나가는 핀을 7번 핀으로 설정
        //처음에 신호선으로 LOW를 18ms동안 그리고 20~40us동안 high신호를 주면 start신호가 된다.
        digitalWrite(DHTPIN,LOW);
        delay(18);//Low 신호를 준뒤 18ms 동안 딜레이
        digitalWrite(DHTPIN,HIGH);
        delayMicroseconds(40);//신호선에 High를 준뒤 40us동안 딜레이
        //라즈베리로 데이터를 받아야 함으로 INPUT으로 설정
        pinMode(DHTPIN,INPUT);

        for(i = 0;i<MAXTIMINGS;i++)
        {
                counter = 0;
                while(digitalRead(DHTPIN)==last_state)//이전 비트와 반전일때 까지 반복문을 돌리며 count를 센다.
                {
                        counter++;
                        delayMicroseconds(1);
                        //이전과 같은 상태로 존재하는 경우에는 count를 증가시켜 1us간격으로 확인한다.
                        if(counter == 255)//255us가 지났을 경우에는 에러가 발생하였다라는  인식을 한후 break을 이용해 반복문을 탈출한다.
                        {
                                break;
                        }
                }

                last_state = digitalRead(DHTPIN);//현재의 값을 이전의 상태로 받는다.

                if(counter == 255)//count가 255이었다면 에러가 발생하였다는 것임으로 반복문을 탈출한다.
                {
                        break;
                }

                if((i>=4)&&(i%2==0))//i가 홀수(Low)가 아니면서 4번째 비트 이후인 비트가 있다면 High 신호가 된다.
                {
                        dht11_dat[j/8]<<=1;//데이터를 쓴다.
                        if(counter>16)//26~28us 일때 data가 0이 되기 때문에 count를 증가시키는 속도와 조건문에서 걸리는 속도(count)가 있기때문에 사실 1ms 가 아니다. 따라서 26이전인 16정도의 count값과 비교
                        {
                                dht11_dat[j/8] |= 1;//1을 쓴다.
                        }
                        j++;//다음 반복 및 데이터 저장을 위하여 j를 1씩 증가시킨다.
                }
    }

        if((j>=40)&&(dht11_dat[4]==((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3])& 0xFF)))//0,1,2,3번을 더한값과 4번값을 비교 (4번의 8비트는 패리트비트)
        {   memset(temp_buf,0,sizeof(temp_buf));
      sprintf(temp_buf,"TeMp%d%d%d%d",dht11_dat[0],dht11_dat[1],dht11_dat[2],dht11_dat[3]);
      write(G_sock,temp_buf,strlen(temp_buf));//온, 습도 데이터인것을 구분하기 위해 TeMp라는 문구와 함께 온, 습도 데이터 출
        }

        else//데이터에 에러가 발생하였다면
        {
              // printf("Data not good.  Please comback attempt \n");//에러문구 출력
        }

}


