#include"Depthprocess.h"
uint16_t fameDepthArray2[MAXLINE];
Imagedepthprocess::Imagedepthprocess()
{
	 _matimg_short.create(240, 320, CV_16UC1);
	 _matimg_show.create(240, 320, CV_8UC1);
}
Imagedepthprocess::~Imagedepthprocess()
{

}
//������ݴ���
//���أ�mat����
Mat Imagedepthprocess::depthProcess()
{
	for (int j = 0; j < bytecount / 2; j++)
	{
		raw_dep = ptr_buf_unsigned[j * 2 + 1] * 256 + ptr_buf_unsigned[2 * j];
		//cout << raw_dep << " ";
		realindex = bytecount / 2 - (j / Img_width + 1) * Img_width + j % Img_width;   //����
		realrow = Img_height - 1 - j / Img_width;
		realcol = j % Img_width;
		fameDepthArray2[realindex] = raw_dep;

	}
	uint16_t depth[240][320];
	for (int i = 0; i < 240; i++)
	{
		for (int j = 0; j < 320; j++)
		{
			depth[i][j] = fameDepthArray2[i * 320 + j];
		}
	}
	//16bitԭʼ����
	for (int i = 0; i < 240; i++)
	{
		for (int j = 0; j < 320; j++)
		{
			_matimg_short.at<ushort>(i, j) = depth[i][j];

		}

	}
	//8bit��ʾ����
	for (int i = 0; i < 240; i++)
	{
		for (int j = 0; j < 320; j++)
		{
			_matimg_show.at<uchar>(i, j) = 255 - _matimg_short.at<ushort>(i, j) * 25 / 3000;
			//cout << src2.at<uchar>(i, j) << endl;
		}

	}
	return _matimg_show.clone();
}
//��ȡ�������
//���أ�Mat����
Mat Imagedepthprocess::getDepth()
{
	return _matimg_show.clone();
}