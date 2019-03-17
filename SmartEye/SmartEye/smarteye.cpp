#include "smarteye.h"



SmartEye::SmartEye(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	getCameraParameterFromFile();	//从config.ini读取相机配置参数

	//点云初始化
	cloud.reset(new PointCloudT);
	cloud->resize(1);

	//点云ui界面元素绑定
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	ui.screen->SetRenderWindow(viewer->getRenderWindow());
	viewer->setupInteractor(ui.screen->GetInteractor(), ui.screen->GetRenderWindow());
	viewer->setCameraPosition(151, -6402, -10771, 0.0133335, 0.945376, -0.325708);	//设置相机视角
	ui.screen->update();	

	ui.screen->hide();	//隐藏点云界面

	g_dcam = new DCam();

	//lable颜色设置
	ui.statelabel->setAlignment(Qt::AlignCenter);	//连接状态剧中
	ui.statelabel->setAutoFillBackground(true);		//填充背景
	QPalette pa;
	pa.setColor(QPalette::Background, Qt::darkRed);
	ui.statelabel->setPalette(pa);					//更改颜色


	//savestatelable颜色设置
	ui.savestatelabel->setAlignment(Qt::AlignCenter);	//连接状态剧中
	ui.savestatelabel->setAutoFillBackground(true);		//填充背景
	ui.savestatelabel->setPalette(pa);					//更改颜色

	//colorimagelabel颜色设置
	ui.colorimagelabel->setAlignment(Qt::AlignCenter);	//连接状态剧中
	ui.colorimagelabel->setAutoFillBackground(true);		//填充背景
	ui.colorimagelabel->setPalette(pa);					//更改颜色

	//图像显示
	//ui.Img_label->setScaledContents(true);

	//槽连接
	QObject::connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(connectButtonPressedSlot()));
	QObject::connect(ui.pclBtn, SIGNAL(clicked()), this, SLOT(pclButtonPressedSlot()));
	QObject::connect(ui.IntegrationtimelineEdit, SIGNAL(editingFinished()), this, SLOT(setIntegrationTime3DSlot()));
	QObject::connect(ui.maxdepthlineEdit, SIGNAL(editingFinished()), this, SLOT(setMappingDistanceSlot()));
	QObject::connect(ui.mindepthlineEdit, SIGNAL(editingFinished()), this, SLOT(setMappingDistanceSlot()));
	QObject::connect(ui.Savebutton, SIGNAL(clicked()), this, SLOT(saveFileSlot()));
	QObject::connect(ui.showpushButton, SIGNAL(releaseMouse()), this, SLOT(showImageSlot()));  //clicked会触发两次
}

SmartEye::~SmartEye()
{
	
}
//通信状态
void SmartEye::connectButtonPressedSlot()
{
	
	if (connectState == 0)
	{
		//启动相机，获取图像
		std::string ip = ui.IplineEdit->text().toStdString();    //获取相机IP
		int port = ui.PortlineEdit->text().toInt();      //获取相机端口号
		g_dcam = new DCam(ip,port);						 //初始化相机类
		connect(g_dcam, SIGNAL(getImage(cv::Mat,int)), this, SLOT(imageUpdateSlot(cv::Mat,int)));	//设置连接槽
		g_dcam->start();	//线程启动
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkYellow);
		ui.statelabel->setPalette(pa);					//更改颜色
		ui.statelabel->setText("Connecting");			//更改文字
		ui.connectButton->setText("Disconnect");
		connectState++;
	}
	else
	{
		g_dcam->setRun(false);

		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkRed);
		ui.statelabel->setPalette(pa);					//更改颜色
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
		connectState--;
	}
	if (connectState == 0)
	{
		saveFileSlot();
	}
	
}

//更新图片槽
//输入：img 传入图像Mat格式
//输入：isImg 是否是图像，避免读取温度造成跳帧
void SmartEye::imageUpdateSlot(cv::Mat img,int isImg)
{
	if (isImg == 1)			//读入图片
	{
		if (img.size().height != 0 && g_dcam->getRunState())		//获取数据正常
		{
			//ui更新
			QPalette pa;
			pa.setColor(QPalette::Background, Qt::darkGreen);
			ui.statelabel->setPalette(pa);					//更改颜色
			ui.statelabel->setText("Connected");
			ui.connectButton->setText("Disconnect");

			//处理原始数据
			cv::Mat imshowsrc = img;
			//显示伪彩色图
			showImage(imshowsrc);
		}
		else							//获取数据失败
		{
			g_dcam->setRun(false);

			//ui更新
			QPalette pa;
			pa.setColor(QPalette::Background, Qt::darkRed);
			ui.statelabel->setPalette(pa);					//更改颜色
			ui.statelabel->setText("No");
			ui.connectButton->setText("Connect");
			return;
		}
	}
	else if (isImg == -1)		//读取异常
	{
		g_dcam->setRun(false);

		//ui更新
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkRed);
		ui.statelabel->setPalette(pa);					//更改颜色
		ui.statelabel->setText("No");
		ui.connectButton->setText("Connect");
		return;
	}
	
	return;
		
}

//点云数据更新
void SmartEye::pointCloudUpdateSlot(PointCloudT::Ptr c)
{
	//点云数据更新
	cloud = c;
	showPointCloud();
}

//QT显示图像
void SmartEye::showImage(Mat imshowsrc)
{
	
	cv::cvtColor(imshowsrc, imshowsrc, CV_BGR2RGB);//Opencv默认BGR存储，Qt需要RGB
	testimage = imshowsrc.clone();
	if (Imageshowstate==1)
	{
		showColorImage();
	}
	QImage img = QImage((uchar*)(imshowsrc.data), imshowsrc.cols, imshowsrc.rows, QImage::Format_RGB888);
	ui.Img_label->setAlignment(Qt::AlignCenter);		//居中显示
	
	
	ui.Img_label->setPixmap(QPixmap::fromImage(img));
	

}

void SmartEye::pclButtonPressedSlot()
{
	//获取畸变矩阵参数
	double fx = ui.FXlineEdit->text().toDouble();
	double fy = ui.FYlineEdit->text().toDouble();
	double cx = ui.CXlineEdit->text().toDouble();
	double cy = ui.CYlineEdit->text().toDouble();
	double k1 = ui.k1lineEdit->text().toDouble();
	double k2 = ui.k2lineEdit->text().toDouble();

	g_dcam->setCameraParameters(fx, fy, cx, cy, k1, k2, 0, 0, 0);


	if (isPCLShow || !(g_dcam->getRunState()))
	{
		//关闭更新
		isPCLShow = false;
		ui.screen->hide();
		g_dcam->setPointcloudConvert(false);
	}
	else
	{
		//显示点云
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
//设置3D积分时间
void SmartEye::setIntegrationTime3DSlot()
{
	g_dcam->integrationtime3D = ui.IntegrationtimelineEdit->text();
	g_dcam->integrationtime3Dflag = 1;
}
//设置映射距离
void SmartEye::setMappingDistanceSlot()
{
	g_dcam->maxdepth = (ui.maxdepthlineEdit->text()).toInt();
	g_dcam->mindepth = (ui.mindepthlineEdit->text()).toInt();
	if (g_dcam->mindepth > g_dcam->maxdepth)
	{
		QMessageBox::information(this, "Error Message", "Please Enter The Correct Format");
	}
}
//保存深度数据
void SmartEye::saveFileSlot()
{
	if (g_dcam->getRunState() == true)
	{
			if (savestate % 2 == 0)
			{
				
				g_dcam->savestr = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly);
				if (g_dcam->savestr.isEmpty())//如果未选择文件便确认，即返回
					return;
				QPalette pac;
				pac.setColor(QPalette::Background, Qt::darkYellow);
				ui.savestatelabel->setPalette(pac);					//更改颜色
				ui.savestatelabel->setText("Saving");			//更改文字
				ui.Savebutton->setText("Dis_save");
				if (savestate == 0)
					savestate++;
				g_dcam->saveimagestate=1;
			}
			else
			{
				QPalette pac;
				pac.setColor(QPalette::Background, Qt::darkRed);
				ui.savestatelabel->setPalette(pac);					//更改颜色
				ui.savestatelabel->setText("Saved");			//更改文字
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
		ui.savestatelabel->setPalette(pac);					//更改颜色
		ui.savestatelabel->setText("Saved");			//更改文字
		ui.Savebutton->setText("save");
		if (savestate == 1)
			savestate--;
		g_dcam->saveimagestate = 0;
	}
	
	

}
// 鼠标单击相应，获取当前深度值
void SmartEye::mousePressEvent(QMouseEvent *event)
{

	Mat img_infor = g_dcam->getDepth();
	//获得相对于屏幕的坐标
	QPoint sPoint1 = event->globalPos();
	//qDebug() << "相对于屏幕坐标1:" << "(" << sPoint1.rx() << "," << sPoint1.ry() << ")";
	//获得相对于控件的坐标
	QPoint widgetPoint = ui.Img_label->mapFromGlobal(sPoint1);
	//qDebug() << "相对于控件坐标:" << "(" << widgetPoint.rx() << "," << widgetPoint.ry() << ")";
	QString image_x;
	QString image_y;
	int img_depth;
	if (sPoint1.rx() >= (sPoint1.rx() - widgetPoint.rx()) & sPoint1.rx() <= (sPoint1.rx() - widgetPoint.rx() + Img_width) & sPoint1.ry() >= (sPoint1.ry() - widgetPoint.ry()) & sPoint1.ry() <= (sPoint1.ry() - widgetPoint.ry() + Img_height))
	{
		//qDebug() << "偏移量:" << "(" << (sPoint1.rx() - widgetPoint.rx()) << "," << (sPoint1.ry()- widgetPoint.ry()) << ")";
		image_x = QString::number(widgetPoint.rx());
		ui.xlineEdit->setText(image_x);
		image_y = QString::number(widgetPoint.ry());
		ui.ylineEdit->setText(image_y);
		img_depth = img_infor.at<ushort>(image_y.toInt(), image_x.toInt());
		QString point_depth = QString::number(img_depth);
		ui.depthlineEdit->setText(point_depth);
		
	}
	else
	{
		//QMessageBox::information(this, "Error Message", "Please Select a Right Place");
	}



}
//弹出新窗口显示伪彩色图像
void SmartEye::showImageSlot()
{
	if (Imageshowstate == 0)
	{
		Imageshowstate = 1;
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkYellow);
		ui.colorimagelabel->setPalette(pa);					//更改颜色
		ui.colorimagelabel->setText("Showing");
		ui.showpushButton->setText("Close");

		showColorImage();
	}
	else
	{
		Imageshowstate = 0;
		QPalette pa;
		pa.setColor(QPalette::Background, Qt::darkRed);
		ui.colorimagelabel->setText("Closed");
		ui.showpushButton->setText("ShowColorImage");
	}
		
	
}
//新窗口显示伪彩色图像
void SmartEye::showColorImage()
{

	imagewin->showimage = testimage.clone();
	imagewin->showColorImage();
	imagewin->show();

}