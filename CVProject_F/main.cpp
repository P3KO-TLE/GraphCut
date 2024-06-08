#include "GraphCut.h"
#include "MMParam.h"

//ԭͼλ��
const string filePath = "D:/CV/Resources/GC2.jpg";
//ԭͼ������
const string winName = "image";
//ԭͼ���ݽṹ
Mat src;
//�������㷨�Ͳ�������ʵ��
GCGraph graph;
MAPMRF params;
//�û������ؼ�
enum { NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
uchar rectState = NOT_SET, lblsState = NOT_SET, prLblsState = NOT_SET;
//�����Ӧ����
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
//��ͼ����
void buildgraph(const Mat& src) {
	//ͼ���������
	int w = src.cols, h = src.rows;
	//param������ʼ��
	cout << "main:param������ʼ����ʼ!\n";
	params.init(src);
	cout << "main:param������ʼ����ɣ�\n";
	//�������ڣ��������Ӧ��������չʾԭͼ
	cout << "main:��ȡ���ָ�����ؿ�ʼ!\n";
	namedWindow(winName, 0);
	resizeWindow(winName, src.cols, src.rows);
	setMouseCallback(winName, on_mouse, 0);
	imshow(winName, src);
	waitKey();
	cout << "main:��ȡ���ָ���������!\n";
	//param��������
	cout << "main:param�������㿪ʼ!\n";
	params.build();
	cout << "main:param�����������!\n";
	//graph��ʼ��
	cout << "main:graph��ʼ����ʼ!\n";
	graph.InitGraph();
	cout << "main:graph��ʼ���ɹ�!\n";
	//graph������ص㡢Դ��ͻ��
	cout << "main:graph��ӽڵ㿪ʼ!\n";
	for (int i = 0; i < h; i++)for (int j = 0; j < w; j++)
		graph.AddNode(src.at<float>(i, j), params.getFBTag(i, j));
	graph.AddS(); graph.AddT();
	cout << "main:graph��ӽڵ�ɹ�!\n";
	//Դ�������ص�����
	cout << "main:graph��ӵ���߿�ʼ!\n";
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
	//���ص���������
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
	//���ص���໥����
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
	cout << "main:graph��ӵ���߳ɹ�!\n";
}
//ͼ��ִ�к���
void doCut(const Mat& src) {
	int w = src.cols, h = src.rows;
	//��������
	cout << "main:graph������㷨��ʼ!\n";
	graph.maxFlow_Dinic();
	cout << "main:graph������㷨�ɹ�!\n";
	//�ɰ���ƽ��ͼ
	cout << "main:graph�����ɰ�����ʼ!\n";
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
	cout << "	���ƽ����"<<cnt<<"��ǰ����\n";
	cout << "main:graph�����ɰ����ɹ�!\n";
	//չʾ����ɰ�
	cout << "main:graphչʾ�ɰ�����ʼ!\n";
	params.showResult();
	cout << "main:graphչʾ�ɰ����ɹ�!\n";
}

int main() {
	//����ͼƬ
	src = imread(filePath, 1);
	//��ͼ
	buildgraph(src);
	//��ͼ��
	doCut(src);

	system("pause");
	return 0;
}