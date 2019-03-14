#ifndef SMARTEYE_H
#define SMARTEYE_H

#include <QtWidgets/QMainWindow>
#include "ui_smarteye.h"
#include<opencv2\opencv.hpp>
#include<thread>
#include<QThread>
#include<Windows.h>
#include<tchar.h>
#include<iostream>
#include <fstream>
#include<direct.h>
#include <io.h>
#include"Depthprocess.h"
#include"TinySocket.h"
#include<qtimer.h>

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
	QTimer    *timer;
	char* sendline = "getDistanceSorted";   //���ͻ�ȡ�������ָ��
	
private slots:
     int TCPSocketSlot();//����TCPͨ��
	 void connectStateSlot();//ͨ��״̬
	 


};

#endif // SMARTEYE_H
