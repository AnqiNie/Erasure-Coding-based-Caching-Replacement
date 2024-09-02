//变式贪心：在不得不替换掉完整的文件块时才替换，否则其他时刻都尽可能不替换完整的块，即使收益高
#include<stdio.h>
#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
int n = 5;//5个服务器
int fn = 6;//六个文件
int m = 2;//至少两块
int r = 3;///每个服务器的缓存空间为3块

int lij[5][5];//服务器之间
int lix[5][6];//服务器与网络
int llimit=200;//最大限制
int pix[5][6];//热度popularity
int qix[5][6];//频次
int lambda[5][6];//替换策略

void request() {
	for (int i = 0; i < 5; i++) {
		/*cout << "在服务器" << i + 1 << "上请求文件1-6：";*/
		for (int j = 0; j < 6; j++) {
			cin >> qix[i][j];
		}
	}
	return ;
}
void calculate_lix() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			for (int t = 0; t < 5; t++) {
				lix[i][j] = max(lix[i][j], lambda[t][j] * lij[t][j]);
			}
		}
	}
	return ;
}
double calculate_cost() {
	double cost = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cost += pix[i][j] * lix[i][j];
		}
	}
	return cost;
}

void show1() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cout << lambda[i][j] << " ";
			if (j == 5) cout << endl;
		}
	}
	return ;
}

void show2() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cout << pix[i][j] << " ";
			if (j == 5) cout << endl;
		}
	}
	return;
}
int main() {
	//为了计算方便 先对服务器的网络延迟情况进行一次手动输入
	cout << "手动输入延迟情况" << endl;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			cin >> lij[i][j];
		}
	}
	//为了计算方便 先对服务器上的文件缓存情况进行一次手动输入
	cout << "手动输入初始缓存情况" << endl;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cin >> lambda[i][j];
		}
	}



	//为了计算方便 先对服务器上的文件热度情况进行一次手动输入
	cout << "手动输入初始热度情况" << endl;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cin >> pix[i][j];
		}
	}


	//接收要求
	cout << "接受请求" << endl;
	request();


	//对每个文件做操作 for each file xn do
	for (int i = 0; i < 6; i++) {

		//统计对第i个文件热度最高的两个服务器 
		int vx = 0, vy = 0;
		for (int j = 0; j < 5; j++) {
			pix[j][i] = pix[j][i]  + qix[j][i] ;
		}
		for (int j = 0; j < 5; j++) {
			if (pix[j][i] > pix[vx][i] && pix[j][i] > pix[vy][i]) {
				vy = vx;
				vx = j;

			}
			else if (pix[j][i] > pix[vx][i] && pix[j][i] <= pix[vy][i]) {
				vy = j;
			}
		}
		//以这两个服务器为中心去找时延限制内可放文件块的服务器作为目标服务器
		calculate_lix();
		int VD[5] = { 0 };
		for (int j = 0; j < 5; j++) {
			if (lix[j][i] < llimit) {
				VD[i] = 1;
			}
		}
		double loss[5][6] = { 0 };
		//开始替换
		for (int j = 0; j < 5; j++) {
			if (VD[j]) {
				for (int t = 0; t < 6; t++) {
					calculate_lix();
					double cost1= calculate_cost();
					lambda[j][i] = 1;
					lambda[j][t] = 0;
					calculate_lix();
					double cost2 = calculate_cost();
					loss[j][t] = cost2-cost1;
					lambda[j][i] = 0;
					lambda[j][t] = 1;
				}
			}
		}
		//找到损失最小的那块
		int vi=0,xi = 0;
		for (int j = 0;j < 5; j++) {
			for (int t = 0; t < 6; t++) {
				if (loss[vi][xi] > loss[j][t]) {
					vi = j;
					xi = t;
				}
			}
		}
		//替换
		lambda[vi][xi] = 0;
		lambda[vi][i] = 1;
	}
	cout << "全部替换后" << endl;
	show1();
	return 0;
}
