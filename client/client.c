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

int main(int argc, char* argv[])
{
   int sock;//Client 와 server를 연결 시킬 소켓을 생성
   struct sockaddr_in serv_addr;//server에 대한 주소를 저장하는 변수 선언
   pthread_t snd_thread, rcv_thread, temp_thread;//쓰레드 변수 선언
   void* thread_return;//pthread_join()에서 쓰레드의 main 함수가 반환하는 값이 저장될 포인터 변수의 주소변수 선언

   if (argc != 4)//입력받은 인수가 3개가 아니라면
   {
      printf("Usage : %s <IP>  <port> <name>\n\n", argv[0]);//실행 문구 출력
      exit(1);//프로그램 종료
   }

   sprintf(name, "[%s]", argv[3]);//마지막에 입력받은 인수를 통하여 Client이름으로 저장
   sock = socket(PF_INET, SOCK_STREAM, 0);//Client 소켓 생성

   memset(&serv_addr, 0, sizeof(serv_addr));//serv_addr 버퍼 초기화
   serv_addr.sin_family = AF_INET;//IPv4 인터넷 프로토콜
   serv_addr.sin_addr.s_addr = inet_addr(argv[1]);//입력받은 IP주소를 server주소로 대입
   serv_addr.sin_port = htons(atoi(argv[2]));//두번째로 받은 인수를 port로 대입

   if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)//server와 연결하는데 문제가 생겼다면
   {
      printf("connect() error\n\n");//경고 문구 출력
      exit(1);//프로그램 종료
   }

   pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);//send_msg에 대한 쓰레드 생성
   pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);//recv_msg에 대한 쓰레드 생성
   pthread_create(&temp_thread, NULL, send_temp, (void*)&sock);//send_temp 에 대한 쓰레드 생성
   pthread_join(snd_thread, &thread_return);
   pthread_join(rcv_thread, &thread_return);
   pthread_join(temp_thread, &thread_return);//쓰레드 사용도중 프로그램이 종료될 수 없으며 각각의 쓰레드 반환

   close(sock);//소켓에 대한 파일을 닫는다
   return 0;
}

