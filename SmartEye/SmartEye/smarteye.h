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
	void showImage();//��ʾͼ��
private:
	Ui::SmartEyeClass ui;
	
private slots:
     void TCPSocketSlot();//����TCPͨ��
	 void connectStateSlot();//ͨ��״̬
	 


};

#endif // SMARTEYE_H
