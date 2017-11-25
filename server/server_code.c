#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 100//최대 99개의 문자 저장가능
#define MAX_CLNT 256//최대 256개의 Client 접속가능

void * snd_total(void* arg);//Server가 입력한 데이터를 모든 Client들에게 전송하는 함수

void send_msg(char * msg, int len);//입력받은 데이터를 모든 Client들에게 roof back 시켜주는 함수


int clnt_cnt=0;//현재 Client의 개수를 저장할 변수 선언
int clnt_socks[MAX_CLNT];//Client들의 소켓 정보를 저장하는 배열 선언
pthread_mutex_t mutx;//뮤텍스 mutx 선언
char send_BUF[BUF_SIZE];//Server가 입력한 데이터를 Client에게 보낼때 사용하는 배열 선언



int main(int argc, char *argv[])
{

	int serv_sock, clnt_sock;//소켓 정보를 저장할 변수 선언
	struct sockaddr_in serv_adr, clnt_adr;//소켓에 대한 주소 구조체 변수 선언
	int clnt_adr_sz;//Client의 주소 구조체 크기를 저장하는 변수 선언
	pthread_t send_id;//쓰레드 변수 선언

	if(argc!=2) {//프로그램 실행시 인수가 받아지지 않았다면
		printf("Usage : %s <port>\n", argv[0]);//해당 실행 명령에 화면에 출력
		exit(1);//프로그램 종료
	}

	pthread_mutex_init(&mutx, NULL);//mutx 초기화
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//Server 소켓 생성

	memset(&serv_adr, 0, sizeof(serv_adr));//Server 통신 설정 변수 초기화
	serv_adr.sin_family=AF_INET;//IPv4인터넷 프로토콜 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);//서버의 IP주소 자동으로 찾아서 대입 
	serv_adr.sin_port=htons(atoi(argv[1]));//인수로 받았던 정수로 변환후 데이터 포트로 설정
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)//IP주소와 PORT 할당중 문제가 생겼다면
	{
		printf("bind() error\n\n");//에러문구 출력
		exit(1);//프로그램 종료
	}

	if(listen(serv_sock, 5)==-1)//연결요청 가능상태로 변경중 문제가 생겼다면 
	{
		printf("listen() error\n\n");//에러문구 출력
		exit(1);//프로그램 종료
	}

	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);//accept()를 하기 위해 Client 소켓의 사이즈를 얻는다
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);//Client의 연결요청에 대한 수락을 한다.
		if(clnt_sock==-1)//연결에 대한 요청이 수락되지 않았다면
		{
			printf("accept() error\n\n");//경고문구 출력
			exit(1);//프로그램 종료
		}
		pthread_mutex_lock(&mutx);//mutex LOCK
		clnt_socks[clnt_cnt++]=clnt_sock;//Client에 대한 소켓정보를 순서대로 누적하여 저장
		pthread_mutex_unlock(&mutx);//mutex UNLOCK
	
		pthread_create(&send_id,NULL,snd_total,(void*)&clnt_sock);//snd_total()에 대한 쓰레드 생성
		pthread_detach(send_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));//해당 네트워크 주소 문자열로 화면에 출력
	}
	close(serv_sock);
	return 0;
}

void* snd_total(void* arg)//Server가 입력한 데이터를 모든 Client들에게 전송하는 함수
{
	int s_idx;//모든 Client들에게 데이터를 전송시켜주기 위한  선언
	memset(send_BUF,0,sizeof(send_BUF));//데이터를 보낼때 사용하는 배열의 버퍼 초기화
	while(1)//무한루프 생성
	{
		fgets(send_BUF,BUF_SIZE,stdin);//키보드의 입력을 배열로 받아온다.
		if(!strcmp(send_BUF,"Q\n")||!strcmp(send_BUF,"q\n"))//만약 입력받은 문자열이 "Q"또는 "q"라면
		{
			exit(1);//프로그램 종료
		}
		pthread_mutex_lock(&mutx);//mutex LOCK
		for(s_idx = 0;s_idx<clnt_cnt;s_idx++)//모든 Client들에게 데이터를 전송하기 위한 반복문 생성
		{
			write(clnt_socks[s_idx],"server>> ",sizeof("server>> "));
			write(clnt_socks[s_idx],send_BUF,strlen(send_BUF));//각각의 Client들에게 데이터를 전송
		}
		pthread_mutex_unlock(&mutx);//mutex UNLOCK
		
	}
}



