#include "smarteye.h"
#include"TinySocket.h"
#include<qtimer.h>

CTinySocket		g_Tcpsocket;
Imagedepthprocess g_depthprocess;
QTimer    *timer;
char sendline[] = "getDistanceSorted";

int k = 0;
cv::Mat imshowsrc;
SmartEye::SmartEye(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//������
	QObject::connect(ui.connectButton, SIGNAL(triggered()), this, SLOT(connectStateSlot()));
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(TCPSocketSlot()));
}

SmartEye::~SmartEye()
{
	
}
//ͨ��״̬
void SmartEye::connectStateSlot()
{
	k++;
	TCPSocketSlot();
}
//TCPͨ��
void SmartEye::TCPSocketSlot()
{
	if (k % 2 == 1)
	{
		g_Tcpsocket._ip = ui.IplineEdit->text().toStdString();    //��ȡ���IP
		g_Tcpsocket._port = ui.PortlineEdit->text().toInt();      //��ȡ����˿ں�
		int flag = g_Tcpsocket.socket_com(sendline, bytecount, (char*)g_Tcpsocket._ip.c_str(), g_Tcpsocket._port);
		if (flag == 1)
		{
			ui.statelabel->setText("Success");
			ui.connectButton->setText("Disconnect");
		}
		g_depthprocess.ptr_buf_unsigned = (unsigned char*)g_Tcpsocket.ptr_buf2;
		imshowsrc=g_depthprocess.depthprocess();
		showImage();
		timer->start();//������ʱ��
	}
	else
	{
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
	}
		
}

//QT��ʾͼ��
void SmartEye::showImage()
{
	QImage img = QImage((const unsigned char*)(imshowsrc.data), imshowsrc.cols, imshowsrc.rows, QImage::Format_Indexed8);
	label = new QLabel();
	ui.Img_label->setAlignment(Qt::AlignCenter);		//������ʾ
	ui.Img_label->setPixmap(QPixmap::fromImage(img));
}

