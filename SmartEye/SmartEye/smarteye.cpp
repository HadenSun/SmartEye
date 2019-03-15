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
	viewer->setCameraPosition(-557.379, 9640.31, -6123, -0.00374522, -0.795958, -0.60534);
	ui.screen->update();

	ui.screen->hide();


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
	
	if (connectState == 0)
	{
		//�����������ȡͼ��
		std::string ip = ui.IplineEdit->text().toStdString();    //��ȡ���IP
		int port = ui.PortlineEdit->text().toInt();      //��ȡ����˿ں�
		g_dcam = new DCam(ip,port);						 //��ʼ�������
		connect(g_dcam, SIGNAL(getImage(cv::Mat)), this, SLOT(imageUpdateSlot(cv::Mat)));	//�������Ӳ�
		g_dcam->start();	//�߳�����

		ui.statelabel->setText("Success");
		ui.connectButton->setText("Disconnect");
		connectState++;
	}
	else
	{
		g_dcam->setRun(false);
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
		connectState--;
	}
	
}
//TCPͨ��
//�������ӶϿ�������0
//�쳣���ӣ�����-1
void SmartEye::imageUpdateSlot(cv::Mat img)
{
	if (img.size().height != 0)
	{
			//����ԭʼ����
			cv::Mat imshowsrc = img;
			//��ʾ�Ҷ�ͼ
			showImage(imshowsrc);
			
			////����չʾ
			//if (isPCLShow)
			//{
			//	cloud = g_pclConvert.getPointCloud(g_depthprocess.getDepth());
			//	showPointCloud();
			//}
	}
	else
	{
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
		return;
	}
	return;
		
}

//�������ݸ���
void SmartEye::pointCloudUpdateSlot(PointCloudT::Ptr c)
{
	//�������ݸ���
	cloud = c;
	showPointCloud();
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

	g_dcam->setCameraParameters(fx, fy, cx, cy, k1, k2, 0, 0, 0);


	if (isPCLShow)
	{
		//�رո���
		isPCLShow = false;
		ui.screen->hide();
		g_dcam->setPointcloudConvert(false);
	}
	else
	{
		//��ʾ����
		isPCLShow = true;
		ui.screen->show();
		connect(g_dcam, SIGNAL(getPointCloud(PointCloudT::Ptr)), this, SLOT(pointCloudUpdateSlot(PointCloudT::Ptr)));
		g_dcam->setPointcloudConvert(true);
	}
}

void SmartEye::showPointCloud()
{
	viewer->removeAllPointClouds();
	viewer->addPointCloud(cloud, "cloud");
	viewer->updatePointCloud(cloud, "cloud");
	//if (!i++)
		//viewer->resetCamera();
	pcl::visualization::Camera c;
	viewer->getCameraParameters(c);
	qDebug() << "pos1:" << c.pos[0] << "\tpos2:" << c.pos[1] <<"\tpos3:"<< c.pos[2] << endl;
	qDebug() << "view1:" << c.view[0] << "\tview2:" << c.view[1] << "\tview3:" << c.view[2] << endl;
	ui.screen->update();
}