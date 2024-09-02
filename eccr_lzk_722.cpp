#include<stdio.h>
#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

int n = 5;//5个服务器
int fn = 6;//6个文件
int m = 2;//至少两块
int r = 3;//最多放3个块
int cloud_lat[6];//从云下载的cost延迟
int netlat[5][5];//网络延迟
int lam[5][6];//策略替换
double p[5][6];//流行度
double q[5][6];//请求频率
int lalimit[5];//每个服务器节点的最大延迟
double limit_theta[5];//累计延迟
double lyapunov = 0;//L
double lyapunov_delta = 0;//delta L
bool file_satis[5][6];
double cost_1;//未优化
double cost_2;//优化
double b_limit = 0;//B上限
double cost_all;
double cost_new;
double benefit;

int edgefile[5][6];//从某个节点出发 获取全部文件块的最大延迟

//逐个检验
bool satis(int x,int y) {
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

//全局检验 如果不满足 
bool satis_all() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (file_satis[i][j] == false&&q[i][j]!=0) return false;
		}
	}
	return true;
}

// 计算cost
int getcost() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cost_1 += p[i][j] * edgefile[i][j];
		}
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (q[i][j] != 0) {
				b_limit += lalimit[i] * lalimit[i];
			}
		}
	}
	b_limit = b_limit / 2;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (q[i][j] != 0) {
				cost_2 += limit_theta[i] * (edgefile[i][j] - lalimit[i]);
			}
		}
	}
	return  cost_1 + b_limit + cost_2;

}
//替换函数
void eccr(int edge,int file) {
	cost_new = 0;
	for (int a = 0; a < 5; a++) {
		for (int b = 0; b < 6; b++) {
			if (lam[a][b] == 1 && a != edge && b != file) {
				lam[a][b] = 0;
				lam[a][file] = 1;
				cost_new = getcost();
				benefit = cost_all - cost_new;
				if (benefit > 0) {
					file_satis[a][file] = satis(a, file);
					file_satis[a][b] = satis(a, b);
					return;
				}
				else {
					lam[a][file] = 0;
					lam[a][b] = 1;
					return;
				}
			}
		}
	}
}

// 实现ECCR-lzk算法

int main() {
	int t = 1;
	int T = 0;
	cout << "输入T轮次" << endl;
	cin >> T;
	cout << "输入服务器间延迟" << endl;//此处不连通网格由于网络传输只考虑要不要超过最大延迟即可 因此本质上不存在不连通网格
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 3; j++) {
			cin >> netlat[i][j];
		}
	}
	cout << "初始情况" << endl;//此处不连通网格由于网络传输只考虑要不要超过最大延迟即可 因此本质上不存在不连通网格
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cin >> lam[i][j];
		}
	}

	while (t <= T) {
		cout << "时刻t= " << t << endl;
		//接受请求
		cout << "每个节点接受请求" << endl;
		for (int i = 0; i < 5; i++) {
			cout << "对节点" << i + 1 << "文件1~6请求次数输入" << endl;
			for (int j = 0; j < 6; j++) {
				cin >> q[i][j];
			}
		}
		cout << "时刻t=" << t << "请求总览：" << endl;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 6; j++) {
				cout << q[i][j] << " ";
				if (j == 5) cout << endl;
			}
		}

		//计算pi,x
		cout << "时刻t=" << t << "计算流行度：" << endl;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 6; j++) {
				p[i][j] = 0.5 * p[i][j] + 0.5 * q[i][j];
			}
		}

		//首先进行全局检验
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 6; j++) {
				file_satis[i][j] = satis(i, j);
			}
		}


		//本简化代码中 由于只有三个服务器节点 每个文件需要两块即可恢复
		//则只有两种情况 请求服务器＋1额外服务器 或者两个额外服务器缓存空间
		//由于每个服务器空间为3，所以四个文件块不可能同时在一个服务器上出现


		//计算li,x
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 6; j++) {
				if (q[i][j] != 0) {
					for (int t = 0; t < 3; t++) {
						edgefile[i][j] = max(edgefile[i][j], lam[t][j] * netlat[i][t]);
					}
				}
			}
		}


		//计算累计延迟
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 6; j++) {
				limit_theta[i] += max(0, edgefile[i][j] - lalimit[i]);
			}
		}
		//计算Lyapunov函数
		int lyapunov_temp = 0;
		for (int i = 0; i < 3; i++) {
			lyapunov_temp += limit_theta[i] * limit_theta[i] / 2;
		}
		lyapunov_delta = lyapunov_temp - lyapunov;
		lyapunov = lyapunov_temp;

		//lyapunov优化后cost
		cost_all = getcost();
		//benifit初始
		int benefit = 0;
		while (!satis_all()) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 6; j++) {
					if (file_satis[i][j] == false) {//开始替换
						eccr(i, j);
					}
				}
			}
		}
	}
	//最终替换后显示每个节点保存文件块
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 6; j++) {
			cout << lam[i][j] << " ";
			if (j == 5) cout << endl;
		}
	}
	return 0;

}
