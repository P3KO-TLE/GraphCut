#include "common.h"

#define MASK_BG_COLOR   128
#define MASK_FG_COLOR   255
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar FG_MASK_COLOR = Scalar(255, 255, 255);
const Scalar BG_MASK_COLOR = Scalar(128, 128, 128);

class MAPMRF {
private:
	//原图指针
	const Mat* src;
	//绘制了选取节点的原图
	Mat imgShow;
	//蒙版图
	Mat mask;
	//图像和mask的大小
	int width, height;
	//相关参数
	float beta, gamma, lambda;
	Mat leftC, upleftC, upC, uprightC;
	float FgHist[3][256], BgHist[3][256];
private:
	//相关参数计算函数
	void CalcBeta();
	void CalcNCost();
	void CalcSeedHist();
	//展示图像函数
	void showImage(Mat src, const string& winName);
public:
	//初始化成员变量
	void init(const Mat& src);
	//计算相关参数接口
	void build();
	//设置并绘画前景点
	void drawFg(Point p,const string & winName);
	//设置并绘画背景点
	void drawBg(Point p,const string &winName);
	//根据象素的颜色获取tcost
	float getTCost(Vec3b color,bool tag);
	//根据象素的坐标获取不同方向的ncost
	float getNCost(int r, int c, int tag);
	//根据象素的坐标获取强约束的标签
	int getFBTag(int r,int c);
	//获取Lambda参数
	float getLambda();
	//reset蒙版
	void resetMask();
	//设置蒙版的前后景点
	void setFg(int r, int c);
	void setBg(int r, int c);
	//展示蒙版结果
	void showResult();
};