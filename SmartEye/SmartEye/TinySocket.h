#define MAXLINE 253600
#define MAX_NUM_PIX	82656	//328 * 252
#define LOW_AMPLITUDE 	32500
#define MAX_PHASE        30000.0
#define MAX_DIST_VALUE 	30000
#define offsetPhaseDefault 0


class CTinySocket
{
 public:
	 ~CTinySocket();
	 int socket_com(char sendline[], int length,const char* destip,const int destport);
	 char buf[MAXLINE];      //���ջ�����
	 char ptr_buf2[MAXLINE];  //�洢������
	 int _port;          
	 string _ip;
private:
	SOCKET  sockfd;
	struct sockaddr_in servaddr;
};
extern "C" void sk_startup(void); //extern "C"��ʵ�ֵ���C��C++�Ļ�ϱ��
extern "C" void sk_cleanup(void);