//ECCR 0705
#include<stdio.h>
#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

// 定义相关结构体和变量

int edgenet[5][5];	 //li,j

int edgefile[5][6] = { 0 };  //li,x

int cloudfile[6];		

double p[5][6] = { 0 };
double q[5][6] = { 0 };

double a ;//pix系数
double b ;//cost系数

int user_request[5][6] = { 0 };

int file_not[5][6] = { 0 };

int lam[5][6];		//answer

double latency[5] = { 0 };		//累计延迟 漂移

int latency_limit[5];

double lyapunov=0;
double lyapunov_delta = 0;

int cost_pl = 0;
int cost_last = 0;
int cost_all = 0;

double b_limit = 0;

// 计算最短路径长度
//此处为简化计算 直接给出两点间最短路径长度

//对节点的某个文件是否满足
bool satis(int x, int y) {
	if (q[x][y] != 0) {//有用户请求 开始检验
		int cnt = 0;
		for (int t = 0; t < 3; t++) {
			if (lam[t][y] != 0) cnt++;
		}
		if (cnt >= 2) return true;
		else return false;
	}
	return true;
}

bool satisfied() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (file_not[i][j] == 1 && q[i][j] != 0) return false;
		}
	}
	return true;
}


// 计算cost

int getcost() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cost_pl += p[i][j] * edgefile[i][j];
		}
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (user_request[i][j] != 0) {
				b_limit += latency_limit[i] * latency_limit[i];
			}
		}
	}
	b_limit = b_limit / 2;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (user_request[i][j] != 0) {
				cost_last += latency[i] * (edgefile[i][j] - latency_limit[i]);
			}
		}
	}
	return b * cost_pl + b_limit + cost_last;

}


// 实现ECCR-lzk算法

int main() {
	int t = 0;
	int T = 0;
	cout << "输入T轮次" << endl;
	cin >> T;
	cout << "输入pi,x系数a" << endl;
	cin >> a;
	cout << "输入cost系数b" << endl;
	cin >> b;
	cout << "输入服务器间延迟" <<endl;//此处不连通网格由于网络传输只考虑要不要超过最大延迟即可 因此本质上不存在不连通网格
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			cin >> edgenet[i][j];
		}
	}
	

	while (t <= T) {
		cout << "时刻t= " << t << endl;
		//接受请求
		cout << "每个节点接受请求" << endl;
		for (int i = 0; i < 5; i++) {
			cout << "对节点" << i + 1 << "文件1~6请求次数输入" << endl;
			for (int j = 0; j < 6; j++) {
				cin >> user_request[i][j] ;
			}
		}
		cout << "时刻t=" << t << "请求总览：" << endl;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				cout << user_request[i][j] << " ";
				if (j == 5) cout << endl;
			}
		}
		//加入qi,x
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				q[i][j] = user_request[i][j];
			}
		}
		
		//计算pi,x
		cout << "时刻t=" << t << "计算流行度：" << endl;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				p[i][j] = a * p[i][j] + (1 - a) * q[i][j];
			}
		}
		
		//计算li，x 并发最大 假设n=3 room=3
		for (int i = 0; i < 5; i++) {
			int cnt = 0;
			for (int j = 0; j < 6; j++) {
				if (user_request[i][j] != 0) {
					for (int t = 0; t < 5; t++) {
						if (lam[t][j] != 0) {
							cnt++;
							if (t != i)
								edgefile[i][j] = max(edgefile[i][j], lam[t][j] * edgenet[i][t]);
						}
					}
					if (cnt < 3) {
						//此处有疑问 如果不达标 edgefile是正无穷还是-1
						edgefile[i][j] = -1;
						file_not[i][j] = 1;
					}
				}
			}
		}

		//计算累计延迟
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 6; j++) {
				latency[i] += max(0, edgefile[i][j] - latency_limit[i]);
			}
		}
		//计算Lyapunov函数
		int lyapunov_temp = 0;
		for (int i = 0; i < 5; i++) {
			lyapunov_temp += latency[i] * latency[i] / 2;
		}
		lyapunov_delta = lyapunov_temp - lyapunov;
		lyapunov = lyapunov_temp;

		//lyapunov优化后cost
		cost_all = getcost();
		//benifit初始
		int benefit = 0;

		//并存执行
		for (int i = 0; i < 5; i++) {
			//满足当下需求
			while (1) {
				for (int j = 0; j < 5; j++) {
					for (int k = 0; k < 6; k++) {
						for (int t = 0; t < 6; t++) {
							int cost_new = 0;//替换
							if (file_not[t] && edgefile[j][k]) {
								lam[j][t] = 1;
								lam[j][k] = 0;
								cost_new = getcost();
								benefit = cost_all - cost_new;
								if (benefit > 0&&satisfied()) continue;
								else {
									lam[j][t] = 0;
									lam[j][k] = 1;
								}
							}
						}
					}
				}
			}
		}
		t = t + 1;
	}
	//最终替换后显示每个节点保存文件块
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cout << edgefile[i][j] << " ";
			if (j == 5) cout << endl;
		}
	}
	return 0;

}

