#include "GraphCut.h"
#include "MMParam.h"

//原图位置
const string filePath = "D:/CV/Resources/GC2.jpg";
//原图窗口名
const string winName = "image";
//原图数据结构
Mat src;
//网络流算法和参数计算实例
GCGraph graph;
MAPMRF params;
//用户交互控件
enum { NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
uchar rectState = NOT_SET, lblsState = NOT_SET, prLblsState = NOT_SET;
//鼠标相应函数
static void on_mouse(int event, int x, int y, int flag, void* param) {
	switch (event) {
	case EVENT_LBUTTONDOWN:
		params.drawFg(Point(x, y), winName);
		lblsState = SET;
		break;
	case EVENT_RBUTTONDOWN:
		params.drawBg(Point(x, y), winName);
		prLblsState = SET;
		break;
	case EVENT_LBUTTONUP:
		lblsState = NOT_SET;
		break;
	case EVENT_RBUTTONUP:
		prLblsState = NOT_SET;
		break;
	case EVENT_MOUSEMOVE:
		if (lblsState != NOT_SET && flag && EVENT_FLAG_LBUTTON)
			params.drawFg(Point(x, y), winName);
		else if (prLblsState != NOT_SET && flag && EVENT_FLAG_RBUTTON)
			params.drawBg(Point(x, y), winName);
		break;
	default:
		lblsState = NOT_SET;
		prLblsState = NOT_SET;
		break;
	}
}
//建图函数
void buildgraph(const Mat& src) {
	//图像基础参数
	int w = src.cols, h = src.rows;
	//param参数初始化
	cout << "main:param参数初始化开始!\n";
	params.init(src);
	cout << "main:param参数初始化完成！\n";
	//创建窗口，绑定鼠标相应函数，并展示原图
	cout << "main:获取鼠标指针像素开始!\n";
	namedWindow(winName, 0);
	resizeWindow(winName, src.cols, src.rows);
	setMouseCallback(winName, on_mouse, 0);
	imshow(winName, src);
	waitKey();
	cout << "main:获取鼠标指针像素完成!\n";
	//param参数计算
	cout << "main:param参数计算开始!\n";
	params.build();
	cout << "main:param参数计算完成!\n";
	//graph初始化
	cout << "main:graph初始化开始!\n";
	graph.InitGraph();
	cout << "main:graph初始化成功!\n";
	//graph添加像素点、源点和汇点
	cout << "main:graph添加节点开始!\n";
	for (int i = 0; i < h; i++)for (int j = 0; j < w; j++)
		graph.AddNode(src.at<float>(i, j), params.getFBTag(i, j));
	graph.AddS(); graph.AddT();
	cout << "main:graph添加节点成功!\n";
	//源点向像素点连边
	cout << "main:graph添加单向边开始!\n";
	int s = graph.getS(), t = graph.getT();
	float lambda = params.getLambda();
	for (int i = 0; i < h; i++)for (int j = 0; j < w; j++) {
		int tag = params.getFBTag(i, j); 
		if (tag == 1) {
			graph.AddEdge(s, i * w + j, lambda);
			graph.AddEdge(i * w + j, s, 0);
		}
		else if (tag == -1) {
			graph.AddEdge(s, i * w + j, 0);
			graph.AddEdge(i * w + j, s, 0);
		}
		else {
			graph.AddEdge(s, i * w + j, params.getTCost(src.at<Vec3b>(i, j), 0));
			graph.AddEdge(i * w + j, s, 0);
		}
	}
	cout << "\n";
	//像素点向汇点连边
	for (int i = 0; i < h; i++)for (int j = 0; j < w; j++) {
		int tag = params.getFBTag(i, j);
		if (tag == 1) {
			graph.AddEdge(i * w + j, t, 0);
			graph.AddEdge(t, i * w + j, 0);
		}
		else if (tag == -1) {
			graph.AddEdge(i * w + j, t, lambda);
			graph.AddEdge(t, i * w + j, 0);
		}
		else {
			graph.AddEdge(i * w + j, t, params.getTCost(src.at<Vec3b>(i, j), 1));
			graph.AddEdge(t, i * w + j, 0);
		}
	}
	//象素点间相互连边
	for (int i = 0; i < h; i++)for (int j = 0; j < w; j++) {
		int id = i * w + h;
		float cost;
		if (j > 0) {
			cost = params.getNCost(i, j, 0);
			graph.AddEdge(id, id - 1, cost);
			graph.AddEdge(id - 1, id, cost);
		}
		if (j > 0 && i > 0) {
			cost = params.getNCost(i, j, 1);
			graph.AddEdge(id, id - w - 1, cost);
			graph.AddEdge(id - w - 1, id, cost);
		}
		if (i > 0) {
			cost = params.getNCost(i, j, 2);
			graph.AddEdge(id, id - w, cost);
			graph.AddEdge(id - w, id, cost);
		}
		if (i > 0 && j < w - 1) {
			cost = params.getNCost(i, j, 3);
			graph.AddEdge(id, id - w + 1, cost);
			graph.AddEdge(id - w + 1, id, cost);
		}
	}
	cout << "main:graph添加单向边成功!\n";
}
//图割执行函数
void doCut(const Mat& src) {
	int w = src.cols, h = src.rows;
	//跑网络流
	cout << "main:graph最大流算法开始!\n";
	graph.maxFlow_Dinic();
	cout << "main:graph最大流算法成功!\n";
	//蒙版绘制结果图
	cout << "main:graph绘制蒙版结果开始!\n";
	params.resetMask();
	int cnt = 0;
	for (int i = 0; i < h; i++)for (int j = 0; j < w; j++) {
		if (graph.getFgTag(i * w + j)) {
			params.setFg(i, j);
			cnt++;
		}
		else {
			params.setBg(i, j);
		}
	}
	cout << "	绘制结果有"<<cnt<<"个前景点\n";
	cout << "main:graph绘制蒙版结果成功!\n";
	//展示结果蒙版
	cout << "main:graph展示蒙版结果开始!\n";
	params.showResult();
	cout << "main:graph展示蒙版结果成功!\n";
}

int main() {
	//读入图片
	src = imread(filePath, 1);
	//建图
	buildgraph(src);
	//作图割
	doCut(src);

	system("pause");
	return 0;
}