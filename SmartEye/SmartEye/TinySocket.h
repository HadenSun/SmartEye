#define MAXLINE 253600
#define MAX_NUM_PIX	82656	//328 * 252
#define LOW_AMPLITUDE 	32500
#define MAX_PHASE        30000.0
#define MAX_DIST_VALUE 	30000
#define offsetPhaseDefault 0
#define bytecount 153600

class CTinySocket
{
 public:
	 void socket_com(char sendline[], int length);
	 char buf[MAXLINE];
	 char ptr_buf2[MAXLINE];
private:
	SOCKET  sockfd;
	struct sockaddr_in servaddr;
};
extern "C" void sk_startup(void); //extern "C"��ʵ�ֵ���C��C++�Ļ�ϱ��
extern "C" void sk_cleanup(void);