#pragma once

#include <stdio.h>
#include <stdint.h>
#include <opencv2\opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <qmessagebox.h>
using namespace std;
using namespace cv;
#define bytecount 153600
#define   Img_width   (320)
#define   Img_height  (240)
#define MAXLINE 253600     
#define MAX_NUM_PIX	82656	//328 * 252
#define LOW_AMPLITUDE 	32500  //ǿ�ȹ���ֵ
#define MAX_PHASE        30000.0 //�¶Ƚ���
#define MAX_DIST_VALUE 	30000 //��Զ����ֵ
#define OFFSET_PHASE_DEFAULT 0  //��Ȳ���ֵ
class Imagedepthprocess
{
public:
	Imagedepthprocess();
	~Imagedepthprocess();
	cv::Mat depthProcess();
	cv::Mat getDepth();
	unsigned char* ptr_buf_unsigned;
	cv::Mat _matimg_short;   //16bit����
	cv::Mat _matimg_show;    //8bit����
	//void calibrate();
private:
	void calibrate(ushort *img);       //�˲�
	void imageAverageEightConnectivity(ushort *depthdata);  //��ֵ�˲�
	void calculationAddOffset(ushort *img);     //��Ȳ���
	int calculationCorrectDRNU(ushort * img);   //�¶Ƚ���

	uint16_t raw_dep;
	int realindex, realrow, realcol;
};