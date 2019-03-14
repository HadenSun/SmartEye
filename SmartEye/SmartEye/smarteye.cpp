#include "smarteye.h"



SmartEye::SmartEye(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//���Ƴ�ʼ��
	cloud.reset(new PointCloudT);
	cloud->resize(1);

	//����ui����Ԫ�ذ�
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	ui.screen->SetRenderWindow(viewer->getRenderWindow());
	viewer->setupInteractor(ui.screen->GetInteractor(), ui.screen->GetRenderWindow());
	viewer->setCameraPosition(114, -500, 44000, 0, 1, 0);
	ui.screen->update();


	//������
	QObject::connect(ui.connectButton, SIGNAL(triggered()), this, SLOT(connectStateSlot()));
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(TCPSocketSlot()));
	QObject::connect(ui.pclBtn, SIGNAL(clicked()), this, SLOT(pointCloudConvert()));
}

SmartEye::~SmartEye()
{
	
}
//ͨ��״̬
void SmartEye::connectStateSlot()
{
	connectState++;
	if (TCPSocketSlot()!=0)
	{
		QMessageBox::information(this, "Error Message", "Connect failed,Please Reconnect");
		connectState--;
	}

	
}
//TCPͨ��
//�������ӶϿ�������0
//�쳣���ӣ�����-1
int SmartEye::TCPSocketSlot()
{
	if (connectState % 2 == 1)
	{
		g_Tcpsocket._ip = ui.IplineEdit->text().toStdString();    //��ȡ���IP
		g_Tcpsocket._port = ui.PortlineEdit->text().toInt();      //��ȡ����˿ں�
		int flag = g_Tcpsocket.socket_com(sendline, bytecount, (char*)g_Tcpsocket._ip.c_str(), g_Tcpsocket._port);
		if (flag == 1)
		{
			ui.statelabel->setText("Success");
			ui.connectButton->setText("Disconnect");
			g_depthprocess.ptr_buf_unsigned = (unsigned char*)g_Tcpsocket.ptr_buf2;
			//����ԭʼ����
			cv::Mat imshowsrc = g_depthprocess.depthProcess();
			//��ʾ�Ҷ�ͼ
			showImage(imshowsrc);
			
			//����չʾ
			if (isPCLShow)
			{
				cloud = g_pclConvert.getPointCloud(g_depthprocess.getDepth());
				showPointCloud();
			}
			

			timer->start();//������ʱ��
		}
		else
		{
			ui.statelabel->setText("No");
			ui.connectButton->setText("Connect");
			return -1;
		}
	}
	else
	{
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
		return 0;
	}
	return 0;
		
}

//QT��ʾͼ��
void SmartEye::showImage(Mat imshowsrc)
{
	QImage img = QImage((const unsigned char*)(imshowsrc.data), imshowsrc.cols, imshowsrc.rows, QImage::Format_Indexed8);
	label = new QLabel();
	ui.Img_label->setAlignment(Qt::AlignCenter);		//������ʾ
	ui.Img_label->setPixmap(QPixmap::fromImage(img));
}

void SmartEye::pointCloudConvert()
{
	//��ȡ����������
	double fx = ui.FXlineEdit->text().toDouble();
	double fy = ui.FYlineEdit->text().toDouble();
	double cx = ui.CXlineEdit->text().toDouble();
	double cy = ui.CYlineEdit->text().toDouble();
	double k1 = ui.k1lineEdit->text().toDouble();
	double k2 = ui.k2lineEdit->text().toDouble();

	g_pclConvert.setConvertParameter(fx, fy, cx, cy, k1, k2,0,0,0);

	if (isPCLShow)
	{
		//�رո���
		isPCLShow = false;
	}
	else
	{
		isPCLShow = true;
	}
}

void SmartEye::showPointCloud()
{
	viewer->removeAllPointClouds();
	viewer->addPointCloud(cloud, "cloud");
	viewer->updatePointCloud(cloud, "cloud");
	if (!i++)
		viewer->resetCamera();
	pcl::visualization::Camera c;
	viewer->getCameraParameters(c);
	qDebug() << "pos1:" << c.pos[0] << "\tpos2:" << c.pos[1] <<"\tpos3:"<< c.pos[2] << endl;
	qDebug() << "view1:" << c.view[0] << "\tview2:" << c.view[1] << "\tview3:" << c.view[2] << endl;
	ui.screen->update();
}