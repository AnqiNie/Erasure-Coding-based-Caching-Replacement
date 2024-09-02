//5个服务器 6个文件 每个服务器缓存为3 每个文件需要3块恢复
//每个服务器最多存1个文件的1个块 
//替换策略为 在尽量不破坏完整性的前提下 按照贪心
//如果不得不破坏完整性 则严格按照贪心
//被破坏完整性的文件选择从云端下载

#include<stdio.h>
#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
int servern = 5;
int fn = 6;
int n = 6;
int m = 3;
int r = 3;

int lij[5][5];
int lix[5][6];
int llimit = 300;
int pix[5][6] = { 0 };
int qix[5][6] = { 0 };
int lambda[5][6] = { 0 };
int VD[5] = {0};

void request() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cin >> qix[i][j];
		}
	}
	return;
}
void calculate_pq() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			pix[i][j] = qix[i][j] + pix[i][j];
		}
	}
	return;
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



bool checkforfile(int x) {
	int cnt = 0;
	for (int i = 0; i < 5; i++) {
		if (lambda[i][x] == 1) cnt += 1;
	}
	if (cnt >= m) return true;
	else return false;
}

bool checkforserver(int v) {
	int cnt = 0;
	for (int i = 0; i < 6; i++) {
		if (lambda[v][i] == 1) cnt += 1;
	}
	if (cnt > r) return false;
	else return true;
}

int calculate_cost() {
	int cost = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			if (checkforfile(j)) {
				cost += pix[i][j] * lix[i][j];
			}
			else {
				cost += 10000;
			}
		}
	}
	return cost;
}

void showout() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			cout << lambda[i][j] << " ";
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
	//每个文件尽可能找到3块
	for (int i = 0; i < 6; i++) {

		//统计对第i个文件热度最高的两个服务器 
		int vx = 0, vy = 0;
		calculate_pq();


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


		for (int a = 0; a < 5; a++) VD[a] = 0;
		for (int j = 0; j < 5; j++) {
			if (lix[j][i] < llimit) {
				VD[i] = 1;
			}
		}

		int loss[5][6] = { 0 };
		//开始替换 
		for (int j = 0; j < 5; j++) {
			if (VD[j]) {
				for (int t = 0; t < 6; t++) {
					if (lambda[j][t] == 1) {
						calculate_lix();
						double cost1 = calculate_cost();
						lambda[j][i] = 1;
						lambda[j][t] = 0;
						calculate_lix();
						double cost2 = calculate_cost();
						loss[j][t] = cost2 - cost1;
						lambda[j][i] = 0;
						lambda[j][t] = 1;
					}
				}
			}
		}
		//找到损失最小的三块
		//先按照损失由小到大进行排序
		//找损失最小的三块 
		//每个都要检验完整性 如果请求了某个文件
		//但是替换该块后破坏了完整性 则进一步考虑从云端下载和缓存二者损失大小
		//即当破坏完整性仍收益较高时 进行替换

		int v1=0, v2=0, v3=0;
		int x1=0, x2=0, x3=0;
		for (int j = 0; j < 5; j++) {
			for (int t = 0; t < 6; t++) {
				if (lambda[j][t]) {
					if (loss[j][t] <= loss[v1][x1]) {
						v3 = v2;
						x3 = x2;
						v2 = v1;
						x2 = x1;
						v1 = j;
						x1 = t;
					}
					else if (loss[j][t] > loss[v1][x1]&& loss[j][t] <= loss[v2][x2]) {
						v3 = v2;
						x3 = x2;
						v2 = j;
						x2 = t;

					}
					else if (loss[j][t] > loss[v1][x1] && loss[j][t] > loss[v2][x2]&& loss[j][t] <= loss[v3][x3]) {
						v3 = j;
						x3 = t;
					}
				}
			}
		}
		lambda[v1][x1] = 0;
		lambda[v2][x2] = 0;
		lambda[v3][x3] = 0;
		lambda[v1][i] = 1;
		lambda[v2][i] = 1;
		lambda[v3][i] = 1;
	}
	cout << "全部替换后" << endl;
	showout();
	int costend = calculate_cost();
	cout << "eccr替换后总成本为：" << costend << endl;
	return 0;
}
