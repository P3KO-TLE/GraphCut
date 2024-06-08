#include "MMParam.h"

void MAPMRF::init(const Mat& src) {
	cout << "	MPAMRF:init(const Mat& src)开始\n";
	this->src = &src;

	width = src.cols, height = src.rows;

	beta = 0, gamma = 50, lambda = 1000;

	mask.create(Size(width, height), CV_8UC1);
	mask.setTo(Scalar::all(GC_BGD));
	namedWindow("mask", 0);

	this->src->copyTo(imgShow);

	leftC.create(Size(width, height), CV_32F);
	upleftC.create(Size(width, height), CV_32F);
	upC.create(Size(width, height), CV_32F);
	uprightC.create(Size(width, height), CV_32F);

	memset(FgHist, 0, sizeof(FgHist));
	memset(BgHist, 0, sizeof(BgHist));

	cout << "	MPAMRF:init(const Mat& src)完成\n";
}

void MAPMRF::CalcBeta() {
	cout << "		MPAMRF:CalcBeta()开始\n";
	beta = 0;
	for (int i = 0; i < height; i++)for (int j = 0; j < width; j++) {
		Vec3f color = (Vec3f)src->at<Vec3b>(i, j);
		if (j > 0) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i, j - 1);
			beta += diff.dot(diff);
		}
		if (j > 0 && i > 0) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i - 1, j - 1);
			beta += diff.dot(diff);
		}
		if (i > 0) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i - 1, j);
			beta += diff.dot(diff);
		}
		if (i > 0 && j < width - 1) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i - 1, j + 1);
			beta += diff.dot(diff);
		}
	}
	if (beta < numeric_limits<double>::epsilon())
		beta = 0;
	else
		beta = 1.0 / (2 * beta / (4 * width * height - 3 * width - 3 * height + 2));
	cout << "		MPAMRF:CalcBeta()完成\n";
}

void MAPMRF::CalcNCost() {
	cout << "		MPAMRF:CalcNCost()开始\n";
	float sqrt2 = sqrt(2.0);
	for (int i = 0; i < height; i++)for (int j = 0; j < width; j++) {
		Vec3f color = (Vec3f)src->at<Vec3b>(i, j);
		if (j - 1 > 0) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i, j - 1);
			leftC.at<float>(i, j) = gamma * exp(-beta * diff.dot(diff));
		}
		else
			leftC.at<float>(i, j) = 0;
		if (j - 1 >= 0 && i - 1 >= 0) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i - 1, j - 1);
			upleftC.at<float>(i, j) = gamma / sqrt2 * exp(-beta * diff.dot(diff));
		}
		else
			upleftC.at<float>(i, j) = 0;
		if (i - 1 > 0) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i - 1, j);
			upC.at<float>(i, j) = gamma * exp(-beta * diff.dot(diff));
		}
		else
			upC.at<float>(i, j) = 0;
		if (i - 1 > 0 && j + 1 < width) {
			Vec3f diff = color - (Vec3f)src->at<Vec3b>(i - 1, j + 1);
			uprightC.at<float>(i, j) = gamma / sqrt2 * exp(-beta * diff.dot(diff));
		}
		else
			uprightC.at<float>(i, j) = 0;
	}
	cout << "		MPAMRF:CalcNCost()完成\n";
}

void MAPMRF::CalcSeedHist() {
	cout << "		MPAMRF:CalcSeedHist()开始\n";
	int nFg = 0, nBg = 0;
	for (int i = 0; i < height; i++)for (int j = 0; j < width; j++) {
		uchar tag = mask.at<uchar>(i, j);
		if (tag == MASK_BG_COLOR) {
			Vec3b color = src->at<Vec3b>(i, j);
			nBg++;
			BgHist[0][color[0]]++;
			BgHist[1][color[1]]++;
			BgHist[2][color[2]]++;
		}
		else if(tag==MASK_FG_COLOR){
			Vec3b color = src->at<Vec3b>(i, j);
			nFg++;
			FgHist[0][color[0]]++;
			FgHist[1][color[1]]++;
			FgHist[2][color[2]]++;
		}
	}

	nFg = nFg > 0 ? nFg : 1;
	nBg = nBg > 0 ? nBg : 1;
	for (int i = 0; i < 3; i++)for (int j = 0; j < 256; j++) {
		FgHist[i][j] = FgHist[i][j] / nFg;
		FgHist[i][j] = FgHist[i][j] < 0.00001 ? 0.00001 : FgHist[i][j];
		BgHist[i][j] = BgHist[i][j] / nBg;
		BgHist[i][j] = BgHist[i][j] < 0.00001 ? 0.00001 : BgHist[i][j];
	}
	cout << "		MPAMRF:CalcSeedHist()完成\n";
}

void MAPMRF::showImage(Mat src, const string& winName) {
	resizeWindow(winName, src.cols, src.rows);
	imshow(winName, src);
}

void MAPMRF::build() {
	cout << "	MPAMRF:build()开始\n";
	CalcBeta();
	CalcNCost();
	CalcSeedHist();
	cout << "	MPAMRF:build()完成\n";
}

void MAPMRF::drawFg(Point p, const string& winName) {
	circle(imgShow, p, 3, BLUE, FILLED, LINE_AA, 0);//划圆
	circle(mask, p, 3, FG_MASK_COLOR, FILLED, LINE_AA, 0);//划圆
	showImage(imgShow, winName);
	showImage(mask, "mask");
}

void MAPMRF::drawBg(Point p, const string& winName) {
	circle(imgShow, p, 3, GREEN, FILLED, LINE_AA, 0);//划圆
	circle(mask, p, 3, BG_MASK_COLOR, FILLED, LINE_AA, 0);//划圆
	showImage(imgShow, winName);
	showImage(mask, "mask");
}

float MAPMRF::getTCost(Vec3b color, bool tag) {
	float p = tag ? FgHist[0][color[0]] * FgHist[1][color[1]] * FgHist[2][color[2]] : BgHist[0][color[0]] * BgHist[1][color[1]] * BgHist[2][color[2]];
	return -1.5 * log(p);
}

float MAPMRF::getNCost(int r,int c, int tag) {
	switch (tag) {
	case 0:
		return leftC.at<float>(r,c);
		break;
	case 1:
		return upleftC.at<float>(r,c);
		break;
	case 2:
		return upC.at<float>(r,c);
		break;
	case 3:
		return uprightC.at<float>(r,c);
		break;
	default:
		break;
	}
	return 0;
}

int MAPMRF::getFBTag(int r,int c) {
	if (mask.at<uchar>(r,c) == MASK_FG_COLOR)
		return 1;
	else if (mask.at<uchar>(r,c) == MASK_BG_COLOR)
		return -1;
	else
		return 0;
}

float MAPMRF::getLambda() {
	return lambda; 
}

void MAPMRF::resetMask() { 
	mask.setTo(GC_BGD); 
}

void MAPMRF::setFg(int r, int c) { 
	mask.at<uchar>(r, c) = MASK_FG_COLOR; 
}

void MAPMRF::setBg(int r, int c) {
	mask.at<uchar>(r, c) = MASK_BG_COLOR;
}

void MAPMRF::showResult() {
	showImage(mask, "mask");
	imwrite("D:/CV/Resources/GCresult.jpg", mask);
	waitKey();
}