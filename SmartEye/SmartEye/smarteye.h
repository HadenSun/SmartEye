#ifndef SMARTEYE_H
#define SMARTEYE_H

#include <vtkAutoInit.h> 
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

#include <QtWidgets/QMainWindow>
#include "ui_smarteye.h"
#include <opencv2\opencv.hpp>
#include <thread>
#include <QThread>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <io.h>
#include "Depthprocess.h"
#include "TinySocket.h"
#include "PCLConvert.h"
#include <qtimer.h>
#include <qdebug.h>

#include "QVTKWidget.h"
#include <vtkRenderWindow.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/common/common.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

using namespace std;
using namespace cv;



class SmartEye : public QMainWindow
{
	Q_OBJECT

public:
	SmartEye(QWidget *parent = 0);
	~SmartEye();
	QLabel *label;
	
	//void depthprocess(); //���ͼ����
	void showImage(Mat imshowsrc);//��ʾͼ��

private:
	Ui::SmartEyeClass ui;
	int connectState = 0;
	CTinySocket		g_Tcpsocket;
	Imagedepthprocess g_depthprocess;
	PCLConvert		g_pclConvert;			//����ת��ʹ��
	QTimer    *timer;
	char* sendline = "getDistanceSorted";   //���ͻ�ȡ�������ָ��
	bool isPCLShow = false;					//�Ƿ����ת����־
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;	//PCL���ӻ�����
	PointCloudT::Ptr cloud;					//����ָ��
	int i = 0;	//��ʱ
	
	void showPointCloud();	  //������ʾ

private slots:
     int TCPSocketSlot();//����TCPͨ��
	 void connectStateSlot();//ͨ��״̬
	 void pointCloudConvert();//����ת������ 
	 

};

#endif // SMARTEYE_H
