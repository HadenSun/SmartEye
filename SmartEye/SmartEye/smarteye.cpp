#include "smarteye.h"



SmartEye::SmartEye(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	getCameraParameterFromFile();	//��config.ini��ȡ������ò���

	//���Ƴ�ʼ��
	cloud.reset(new PointCloudT);
	cloud->resize(1);

	//����ui����Ԫ�ذ�
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	ui.screen->SetRenderWindow(viewer->getRenderWindow());
	viewer->setupInteractor(ui.screen->GetInteractor(), ui.screen->GetRenderWindow());
	viewer->setCameraPosition(151, -6402, -10771, 0.0133335, 0.945376, -0.325708);	//��������ӽ�
	ui.screen->update();	

	ui.screen->hide();	//���ص��ƽ���

	g_dcam = new DCam();

	//lable��ɫ����
	ui.statelabel->setAlignment(Qt::AlignCenter);	//����״̬����
	ui.statelabel->setAutoFillBackground(true);		//��䱳��
	QPalette pa;
	pa.setColor(QPalette::Background, Qt::darkRed);
	ui.statelabel->setPalette(pa);					//������ɫ


	//������
	QObject::connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(connectButtonPressedSlot()));
	QObject::connect(ui.pclBtn, SIGNAL(clicked()), this, SLOT(pclButtonPressedSlot()));
	QObject::connect(ui.IntegrationtimelineEdit, SIGNAL(editingFinished()), this, SLOT(setIntegrationTime3DSlot()));
}

SmartEye::~SmartEye()
{
	
}
//ͨ��״̬
void SmartEye::connectButtonPressedSlot()
{
	
	if (connectState == 0)
	{
		//�����������ȡͼ��
		std::string ip = ui.IplineEdit->text().toStdString();    //��ȡ���IP
		int port = ui.PortlineEdit->text().toInt();      //��ȡ����˿ں�
		g_dcam = new DCam(ip,port);						 //��ʼ�������
		connect(g_dcam, SIGNAL(getImage(cv::Mat,int)), this, SLOT(imageUpdateSlot(cv::Mat,int)));	//�������Ӳ�
		g_dcam->start();	//�߳�����
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkYellow);
		ui.statelabel->setPalette(pa);					//������ɫ
		ui.statelabel->setText("Connecting");			//��������
		ui.connectButton->setText("Disconnect");
		connectState++;
	}
	else
	{
		g_dcam->setRun(false);

		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkRed);
		ui.statelabel->setPalette(pa);					//������ɫ
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
		connectState--;
	}
	
}

//����ͼƬ��
//���룺img ����ͼ��Mat��ʽ
//���룺isImg �Ƿ���ͼ�񣬱����ȡ�¶������֡
void SmartEye::imageUpdateSlot(cv::Mat img,int isImg)
{
	if (isImg == 1)			//����ͼƬ
	{
		if (img.size().height != 0 && g_dcam->getRunState())		//��ȡ��������
		{
			//ui����
			QPalette pa;
			pa.setColor(QPalette::Background, Qt::darkGreen);
			ui.statelabel->setPalette(pa);					//������ɫ
			ui.statelabel->setText("Connected");
			ui.connectButton->setText("Disconnect");

			//����ԭʼ����
			cv::Mat imshowsrc = img;
			//��ʾ�Ҷ�ͼ
			showImage(imshowsrc);
		}
		else							//��ȡ����ʧ��
		{
			g_dcam->setRun(false);

			//ui����
			QPalette pa;
			pa.setColor(QPalette::Background, Qt::darkRed);
			ui.statelabel->setPalette(pa);					//������ɫ
			ui.statelabel->setText("No");
			ui.connectButton->setText("Connect");
			return;
		}
	}
	else if (isImg == -1)		//��ȡ�쳣
	{
		g_dcam->setRun(false);

		//ui����
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkRed);
		ui.statelabel->setPalette(pa);					//������ɫ
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

void SmartEye::pclButtonPressedSlot()
{
	//��ȡ����������
	double fx = ui.FXlineEdit->text().toDouble();
	double fy = ui.FYlineEdit->text().toDouble();
	double cx = ui.CXlineEdit->text().toDouble();
	double cy = ui.CYlineEdit->text().toDouble();
	double k1 = ui.k1lineEdit->text().toDouble();
	double k2 = ui.k2lineEdit->text().toDouble();

	g_dcam->setCameraParameters(fx, fy, cx, cy, k1, k2, 0, 0, 0);


	if (isPCLShow || !(g_dcam->getRunState()))
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
	ui.screen->update();
}

void SmartEye::getCameraParameterFromFile()
{
	QFile file("config.ini");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		int count = 0;
		while (!file.atEnd())
		{
			QByteArray line = file.readLine();
			if (line[0] == '\n')
				break;

			if (line[0] != '#')
			{
				QString str(line);
				switch (count++)
				{
				case 0: ui.FXlineEdit->setText(str); break;
				case 1: ui.FYlineEdit->setText(str); break;
				case 2: ui.CXlineEdit->setText(str); break;
				case 3: ui.CYlineEdit->setText(str); break;
				case 5: ui.k1lineEdit->setText(str); break;
				case 6: ui.k2lineEdit->setText(str); break;
				default:
					break;
				}
			}
		}
		file.close();
	}
}
//����3D����ʱ��
void SmartEye::setIntegrationTime3DSlot()
{
	g_dcam->integrationtime3D = ui.IntegrationtimelineEdit->text();
	g_dcam->integrationtime3Dflag = 1;
}