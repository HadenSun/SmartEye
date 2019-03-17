#include "image.h"

Image::Image(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//����ͼ��Ŵ���С
	ui.imagelabel->setScaledContents(true);
}

Image::~Image()
{

}
//��ʾα��ɫͼ��
void Image::showColorImage()
{
	QImage img = QImage((uchar*)(showimage.data), showimage.cols, showimage.rows, QImage::Format_RGB888);
	QImage image2 = img.scaled(320, 240, Qt::KeepAspectRatio);
	ui.imagelabel->setAlignment(Qt::AlignCenter);		//������ʾ
	ui.imagelabel->setPixmap(QPixmap::fromImage(image2));
}