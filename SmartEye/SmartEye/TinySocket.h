
#include <stdio.h>
#include<string>
#include<iostream>


#define MAXLINE 253600
#define MAX_NUM_PIX	82656	//328 * 252
#define LOW_AMPLITUDE 	32500
#define MAX_PHASE        30000.0
#define MAX_DIST_VALUE 	30000
#define offsetPhaseDefault 0

using namespace std;
class CTinySocket
{
 public:
	 CTinySocket();
	 ~CTinySocket();
	 int socket_com(char sendline[], int length,const char* destip,const int destport);
	 char buf[MAXLINE];      //���ջ�����
	 char ptr_buf2[MAXLINE];  //�洢������
	 int _port;          
	 string _ip;
private:
	int  sockfd;
	
};
extern "C" void sk_startup(void); //extern "C"��ʵ�ֵ���C��C++�Ļ�ϱ��
extern "C" void sk_cleanup(void);