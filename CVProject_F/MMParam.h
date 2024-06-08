#include "common.h"

#define MASK_BG_COLOR   128
#define MASK_FG_COLOR   255
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar FG_MASK_COLOR = Scalar(255, 255, 255);
const Scalar BG_MASK_COLOR = Scalar(128, 128, 128);

class MAPMRF {
private:
	//ԭͼָ��
	const Mat* src;
	//������ѡȡ�ڵ��ԭͼ
	Mat imgShow;
	//�ɰ�ͼ
	Mat mask;
	//ͼ���mask�Ĵ�С
	int width, height;
	//��ز���
	float beta, gamma, lambda;
	Mat leftC, upleftC, upC, uprightC;
	float FgHist[3][256], BgHist[3][256];
private:
	//��ز������㺯��
	void CalcBeta();
	void CalcNCost();
	void CalcSeedHist();
	//չʾͼ����
	void showImage(Mat src, const string& winName);
public:
	//��ʼ����Ա����
	void init(const Mat& src);
	//������ز����ӿ�
	void build();
	//���ò��滭ǰ����
	void drawFg(Point p,const string & winName);
	//���ò��滭������
	void drawBg(Point p,const string &winName);
	//�������ص���ɫ��ȡtcost
	float getTCost(Vec3b color,bool tag);
	//�������ص������ȡ��ͬ�����ncost
	float getNCost(int r, int c, int tag);
	//�������ص������ȡǿԼ���ı�ǩ
	int getFBTag(int r,int c);
	//��ȡLambda����
	float getLambda();
	//reset�ɰ�
	void resetMask();
	//�����ɰ��ǰ�󾰵�
	void setFg(int r, int c);
	void setBg(int r, int c);
	//չʾ�ɰ���
	void showResult();
};