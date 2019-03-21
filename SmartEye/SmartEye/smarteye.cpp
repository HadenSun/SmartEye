#include "smarteye.h"



SmartEye::SmartEye(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.pointCloudDock->hide();	//���ص��ƽ���
	ui.imageDock->hide();		//����ͼ�����

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
	
	g_dcam = new DCam();

	//lable��ɫ����
	ui.statelabel->setAlignment(Qt::AlignCenter);	//����״̬����
	ui.statelabel->setAutoFillBackground(true);		//��䱳��
	QPalette pa;
	pa.setColor(QPalette::Background, Qt::darkRed);
	ui.statelabel->setPalette(pa);					//������ɫ
	
	//savestatelable��ɫ����
	ui.savestatelabel->setAlignment(Qt::AlignCenter);	//����״̬����
	ui.savestatelabel->setAutoFillBackground(true);		//��䱳��
	ui.savestatelabel->setPalette(pa);					//������ɫ

	//���ô��ڴ�С
	this->setFixedSize(408, 408);

	//ͼ�����¼�
	ui.Img_label->installEventFilter(this);		//label����¼������eventFilter����


	//������
	QObject::connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(connectButtonPressedSlot()));
	QObject::connect(ui.pclBtn, SIGNAL(clicked()), this, SLOT(pclButtonPressedSlot()));
	QObject::connect(ui.IntegrationtimelineEdit, SIGNAL(editingFinished()), this, SLOT(setIntegrationTime3DSlot()));
	QObject::connect(ui.maxdepthlineEdit, SIGNAL(editingFinished()), this, SLOT(setMappingDistanceSlot()));
	QObject::connect(ui.mindepthlineEdit, SIGNAL(editingFinished()), this, SLOT(setMappingDistanceSlot()));
	QObject::connect(ui.Savebutton, SIGNAL(clicked()), this, SLOT(saveFileSlot()));
	QObject::connect(ui.pointSizeSlider, SIGNAL(sliderReleased()), this, SLOT(pointSizeSliderReleaseSlot()));
	
}

SmartEye::~SmartEye()
{
	
}

//���Ӱ�ť����¼���
void SmartEye::connectButtonPressedSlot()
{
	if (connectState == 0)
	{
		//�����������ȡͼ��
		std::string ip = ui.IplineEdit->text().toStdString();    //��ȡ���IP
		int port = ui.PortlineEdit->text().toInt();      //��ȡ����˿ں�
		g_dcam->setNet(ip,port);						 //��ʼ�������
		connect(g_dcam, SIGNAL(getImage(cv::Mat,int)), this, SLOT(imageUpdateSlot(cv::Mat,int)));	//�������Ӳ�
		g_dcam->start();	//�߳�����
		
		//��ť״̬�ı�
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkYellow);
		ui.statelabel->setPalette(pa);					//������ɫ
		ui.statelabel->setText("Connecting");			//��������
		ui.connectButton->setText("Disconnect");

		//dock��ʾ
		ui.imageDock->show();

		connectState++;
	}
	else
	{
		g_dcam->setRun(false);

		//��ť״̬�ı�
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkRed);
		ui.statelabel->setPalette(pa);					//������ɫ
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");

		disconnect(g_dcam, SIGNAL(cvGetImage(cv::Mat, int)), this, SLOT(imageUpdateSlot(cv::Mat, int)));	//�Ͽ���

		//dock����
		ui.imageDock->hide();

		connectState--;
	}

	if (connectState == 0)
	{
		saveFileSlot();				//��������
		pclButtonPressedSlot();		//����������ʾ
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
			//��ʾα��ɫͼ
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
//���룺c ����ָ��
void SmartEye::pointCloudUpdateSlot(PointCloudT::Ptr c)
{
	//�������ݸ���
	cloud = c;
	showPointCloud();
}

//QT��ʾͼ��
//���룺imshowsrc α��ɫͼ��
void SmartEye::showImage(Mat imshowsrc)
{
	Mat imgShow = imshowsrc.clone();
	cv::cvtColor(imgShow, imgShow, CV_BGR2RGB);//OpencvĬ��BGR�洢��Qt��ҪRGB
	QImage img = QImage((uchar*)(imgShow.data), imgShow.cols, imgShow.rows, QImage::Format_RGB888);
	
	int width = ui.Img_label->size().width();
	int height = ui.Img_label->size().height();
	//����Ӧ�����
	if ((height / 240.0) > (width / 320.0))
	{
		//���տ������С
		height = width / 320.0 * 240.0;
	}
	else
	{
		//���ոߵ�����С
		width = height / 240.0 * 320.0;
	}

	img = img.scaled(width,height, Qt::KeepAspectRatio, Qt::SmoothTransformation);	//ͼ������
	ui.Img_label->setAlignment(Qt::AlignCenter);		//������ʾ
	ui.Img_label->setPixmap(QPixmap::fromImage(img));	//����ͼ��
	

}

//����ת����ť����¼���
void SmartEye::pclButtonPressedSlot()
{
	//��ȡ����������
	double fx = ui.FXlineEdit->text().toDouble();
	double fy = ui.FYlineEdit->text().toDouble();
	double cx = ui.CXlineEdit->text().toDouble();
	double cy = ui.CYlineEdit->text().toDouble();
	double k1 = ui.k1lineEdit->text().toDouble();
	double k2 = ui.k2lineEdit->text().toDouble();

	//���û���ϵ��
	g_dcam->setCameraParameters(fx, fy, cx, cy, k1, k2, 0, 0, 0);


	if (isPCLShow || !(g_dcam->getRunState()))
	{
		//�رո���
		isPCLShow = false;
		ui.pointCloudDock->hide();		//dock����
		disconnect(g_dcam, SIGNAL(getPointCloud(PointCloudT::Ptr)), this, SLOT(pointCloudUpdateSlot(PointCloudT::Ptr)));	//�Ͽ�������
		g_dcam->setPointcloudConvert(false);
	}
	else
	{
		//��ʾ����
		isPCLShow = true;
		ui.pointCloudDock->show();		//dock��ʾ
		connect(g_dcam, SIGNAL(getPointCloud(PointCloudT::Ptr)), this, SLOT(pointCloudUpdateSlot(PointCloudT::Ptr)));		//���ò�����
		g_dcam->setPointcloudConvert(true);
	}
}

//���ƽ������
void SmartEye::showPointCloud()
{
	viewer->removeAllPointClouds();
	viewer->addPointCloud(cloud, "cloud");
	viewer->updatePointCloud(cloud, "cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, pointSize, "cloud");
	ui.screen->update();
}

//��config�ļ���ȡ�������
void SmartEye::getCameraParameterFromFile()
{
	QFile file("config.ini");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		int count = 0;
		while (!file.atEnd())
		{
			//��ȡһ������
			QByteArray line = file.readLine();
			if (line[0] == '\n')
				break;

			//����ע��
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

//����ӳ�����
void SmartEye::setMappingDistanceSlot()
{
	int maxDepth = (ui.maxdepthlineEdit->text()).toInt();
	int minDepth = (ui.mindepthlineEdit->text()).toInt();

	//�������Χ
	if (maxDepth > COLORMAP_MAX_DEPTH)
	{
		maxDepth = COLORMAP_MAX_DEPTH;
		ui.maxdepthlineEdit->setText(QString::number(COLORMAP_MAX_DEPTH));
	}
	
	//������С��Χ
	if (minDepth < COLORMAP_MIN_DEPTH)
	{
		minDepth = COLORMAP_MIN_DEPTH;
		ui.mindepthlineEdit->setText(QString::number(COLORMAP_MIN_DEPTH));
	}

	g_dcam->maxdepth = maxDepth;
	g_dcam->mindepth = minDepth;

	
	//������������
	if (g_dcam->mindepth > g_dcam->maxdepth)
	{
		QMessageBox::information(this, "Error Message", "Please Enter The Correct Format");
	}
}

//�����������
void SmartEye::saveFileSlot()
{
	if (g_dcam->getRunState() == true)
	{
			if (savestate % 2 == 0)
			{
				
				g_dcam->savestr = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly);
				if (g_dcam->savestr.isEmpty())//���δѡ���ļ���ȷ�ϣ�������
					return;
				QPalette pac;
				pac.setColor(QPalette::Background, Qt::darkYellow);
				ui.savestatelabel->setPalette(pac);					//������ɫ
				ui.savestatelabel->setText("Saving");			//��������
				ui.Savebutton->setText("Dis_save");
				if (savestate == 0)
					savestate++;
				g_dcam->saveimagestate=1;
			}
			else
			{
				QPalette pac;
				pac.setColor(QPalette::Background, Qt::darkRed);
				ui.savestatelabel->setPalette(pac);					//������ɫ
				ui.savestatelabel->setText("Saved");			//��������
				ui.Savebutton->setText("save");
				if (savestate == 1)
					savestate--;
				g_dcam->saveimagestate = 0;
			}
		
	}
	else
	{
		QPalette pac;
		pac.setColor(QPalette::Background, Qt::darkRed);
		ui.savestatelabel->setPalette(pac);					//������ɫ
		ui.savestatelabel->setText("Saved");			//��������
		ui.Savebutton->setText("save");
		if (savestate == 1)
			savestate--;
		g_dcam->saveimagestate = 0;
	}
	
	

}

//label����¼�������
bool SmartEye::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *env = static_cast<QMouseEvent *>(e);
		if (ui.Img_label == obj)
		{
			//label�������ȡͼ������
			int img_x = env->x() * 320 / ui.Img_label->size().width();
			int img_y = env->y() * 240 / ui.Img_label->size().height();

			//��ȫ���˼��
			if (img_x >= 320)
				img_x = 319;
			if (img_x < 0)
				img_x = 0;
			if (img_y >= 240)
				img_y = 239;
			if (img_y < 0)
				img_y = 0;

			//ui��ʾ
			ui.xlineEdit->setText(QString::number(img_x));
			ui.ylineEdit->setText(QString::number(img_y));
			//ȷ��ͼ��������ֵ
			Mat img_infor = g_dcam->getDepth();
			int point_depth = img_infor.at<ushort>(img_y, img_x);
			ui.depthlineEdit->setText(QString::number(point_depth));

			return true;		//��ֹ�¼�����
		}
	}
	
	return false;				//�¼�����������������
}


void SmartEye::pointSizeSliderReleaseSlot()
{
	pointSize = ui.pointSizeSlider->value();
	showPointCloud();
}
