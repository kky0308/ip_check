
#include<stdint.h>
#include<fcntl.h>
#include<termios.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<signal.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

#define MAXTIMINGS 83

void* send_msg(void* arg);//데이터를 server로 전송하는 함수
void* recv_msg(void* arg);//server에서 전송된 데이터를 받는 함수


char name[NAME_SIZE] = "[DEFAULT]";//
char msg[BUF_SIZE];//메세지를 저장할 배열 선언
int G_sock;//소켓을 전역변수로 전환하는 변수 선언



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
   pthread_join(snd_thread, &thread_return);
   pthread_join(rcv_thread, &thread_return);//쓰레드 사용도중 프로그램이 종료될 수 없으며 각각의 쓰레드 반환

   close(sock);//소켓에 대한 파일을 닫는다
   return 0;
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


