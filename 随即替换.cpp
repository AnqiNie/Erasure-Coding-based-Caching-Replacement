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
#include<cstdlib>
#include<ctime>
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
int VD[5] = { 0 };

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
	return;
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
				cost += 10000;				//此处为了突出缓存快 云下载慢 故放大
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
	srand((unsigned)time(NULL));
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

	//随机替换 随机生成三个数对应 服务器 然后替换掉其中的某一块
	for (int i = 0; i < 6; i++) {
		int vrandom1 = (rand()%5);
		int vrandom2 = (rand() % 5);
		int vrandom3 = (rand() % 5);
		while (vrandom2 == vrandom1) {
			vrandom2 = (rand() % 5);
		}
		while (vrandom3 == vrandom1||vrandom3==vrandom2) {
			vrandom3 = (rand() % 5);
		}
		for (int t = 0; t < 6; t++) {
			if (lambda[vrandom1][t] == 1) {
				lambda[vrandom1][t] = 0;
				lambda[vrandom1][i] = 1;
			}
			if (lambda[vrandom2][t] == 1) {
				lambda[vrandom2][t] = 0;
				lambda[vrandom2][i] = 1;
			}
			if (lambda[vrandom3][t] == 1) {
				lambda[vrandom3][t] = 0;
				lambda[vrandom3][i] = 1;
			}
		}
	}
	cout << "全部替换后" << endl;
	showout();
	calculate_pq();
	calculate_lix();
	int costend = calculate_cost();
	cout << "随机替换后总成本为：" << costend << endl;
	return 0;
}
