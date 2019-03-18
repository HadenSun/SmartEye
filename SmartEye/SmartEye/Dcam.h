#pragma once

#include <QThread>
#include <opencv2/opencv.hpp>
#include <string>
#include<qstring.h>
#include <QMetaType>
#include<qdebug.h>

#include "TinySocket.h"
#include "Depthprocess.h"
#include "PCLConvert.h"


#define COLORMAP_MAX_DEPTH 30000
#define COLORMAP_MIN_DEPTH 0


class DCam : public QThread
{
	Q_OBJECT

public:
	DCam(QObject *parent);
	DCam();
	DCam(std::string ip, int port);
	~DCam();
	void setRun(bool isRun);				//�����߳�ֹͣ
	void setNet(std::string ip, int port);	//�������IP���˿�
	void setPointcloudConvert(bool isConvert);	//���õ�����ʾ
	Mat getDepth();                          //��ȡ���ͼ��
	void setCameraParameters(double fx, double fy, double cx, double cy, double k1, double k2, double p1, double p2, double k3);	//��������ڲΡ�����ϵ��
	int setrealtemperature(char *buf);    //��ȡ����¶�
	int maxdepth=30000;           //ӳ����Զ����    
	int mindepth=0;           //ӳ���������
	bool getRunState();						//��ȡ����״̬
	int saveimagestate = 0;                         //save״̬
	bool  integrationtime3Dflag = 0;     //����ʱ���־λ
	QString integrationtime3D="0";   //�������ʱ��
	QString  savestr;                 //����·��
	cv::Mat dcam_imageinfor;             //���ͼ��

signals:
	void getImage(cv::Mat,int);				//��ȡͼ����ź�,Mat��ʽ����ͼ����Ϣ��int�����Ƿ���ͼ��0����ͼ��1��ͼ��-1�쳣
	void getPointCloud(PointCloudT::Ptr);	//��ȡ�����ź�

protected:
	void run();				//�̳���QThread���߳����к���

private:
	Imagedepthprocess g_depthprocess;	//ԭʼͼ������
	CTinySocket	g_Tcpsocket;			//SOCKET��
	PCLConvert	g_pclConvert;			//����ת��ʹ��
	bool isRun = false;					//�Ƿ�����
	bool isPointCloudConvert = false;	//�Ƿ����ת��
	std::string ip = "192.168.7.2";		//���IP
	int port = 50660;					//����˿�
	int g_TempReadDelay = 0;                 //��ȡ����¶��ӳ��ź�
	int g_TempReadEnable = 0;                //��ȡ����¶��ź�
	char* send_distance = "getDistanceSorted";   //���ͻ�ȡ�������ָ��
	char* send_temp = "getTemperature";//���ͻ�ȡ�¶�����ָ��
	QString send_integrationtime3D = "setIntegrationTime3D ";   //����3D����ʱ��ָ��
	ushort	realTempChip;		//�¶�
	ushort	realTempBoard1;
	ushort	realTempBoard2;
	

};

