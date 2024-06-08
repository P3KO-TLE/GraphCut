#include"common.h"

class GCGraph {
	//�ڵ�������ڵ�����Ĺ�ϵ:id=r*w+c
private:
	//INT�������
	const int INT_INF = 1e9;
	//�ڵ�ṹ��
	struct gnode {
		float gray;
		int tag;
	};
	//����߽ṹ��
	struct gedge {
		float val;
		int to;
		int nxt;
	};
	//�ڵ�ʵ��
	vector<gnode> node;
	//�����ʵ��
	vector<gedge> edge;
	//�ڽ�����ͷ
	vector<int> head;
	//�����
	float maxflow;
	//Դ��ͻ��
	int s, t;
	//�ڵ����������������
	int nodes, edges;
	//dinic�㷨��ص����ݽṹ
	vector<int>deep;
	vector<int>now;
	queue<int>q;
public:
	//��ʼ����Ա����
	void InitGraph();
	//��ӽڵ�
	void AddNode(float gray, int tag);
	//��ӵ����
	void AddEdge(int from, int to, float val);
	//���Դ��
	void AddS();
	//��ӻ��
	void AddT();
	//��ȡԴ������
	int getS();
	//��ȡ�������
	int getT();
	//��ȡ�����
	float getFlow();
	//Dinic�㷨����������
	void maxFlow_Dinic();
	//ͨ���ڵ�������ȡ��ǩ
	bool getFgTag(int ntag);
private:
	//Dinic�㷨��غ���
	bool bfs();
	float dfs(int x, float flow);
};