//5个服务器 6个文件 每个服务器缓存为3 每个文件需要3块恢复
//每个服务器最多存1个文件的1个块 
//替换策略为 在尽量不破坏完整性的前提下 按照贪心
//如果不得不破坏完整性 则严格按照贪心
//被破坏完整性的文件选择从云端下载

//每个文件被分为多少块？？

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

//每个服务器之间延迟情况,不能直接相连的是无穷
int lij[5][5] = { {0,200,INFINITY,INFINITY,100},{200,0,300,100,200},{INFINITY,300,0,400,INFINITY},
	{INFINITY,100,400,0,200},{100,200,INFINITY,200,0} };
int lix[5][6];//是不是想算每个服务器获取不同文件所需延迟？
int llimit = 300;
//文件热度情况
int pix[5][6] = { {1,2,5,6,9,7},{5,6,3,5,8,7},{5,3,6,9,7,5},{8,6,2,3,3,4},{6,2,6,4,3,1} };
int qix[5][6] = {0 };
//服务器上文件缓存情况
int lambda[5][6] = { {1,0,1,1,1,1},{1,1,1,1,0,1},{1,1,1,0,0,1},{1,1,1,0,1,0},{0,0,1,1,1,0} };
//可替换文件的目标服务器
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

//void calculate_lix() {
//	for (int i = 0; i < 5; i++) {
//		for (int j = 0; j < 6; j++) {
//			for (int t = 0; t < 5; t++) {
//				lix[i][j] = max(lix[i][j], lambda[t][j] * lij[t][j]);
//			}
//		}
//	}
//	return ;
//}



//这个calculate_lix()有问题，每个文件只需要三块就可以恢复，所以不需要找出所有块的最高延迟，只需找出最近三块的最高延迟即可
void calculate_lix() {
	int l[5] = { 0 };//用于记录对于服务器i要获取到j文件每一块对应的各个时间延迟
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) {
			for (int t = 0; t < 5; t++) {
				if (lambda[t][j] == 0) {
					l[t] = INFINITY;
				}
				else {
					l[t] = lij[i][t];
				}
				
			}
			//在数组l【5】中找出最近三块的最高延迟即可，使用选择排序
			if (lambda[i][j] == 0) {
				//说明在本地服务器上没有该文件块,选择排序选出三块
				for (int k = 0; k < 3; k++) {
					for (int m = k + 1; m < 5; m++) {
						if (l[k] > l[m]) {
							int temp = 0;
							temp = l[k];
							l[k] = l[m];
							l[m] = temp;
						}
					}
				}
				lix[i][j] = l[2];
			}
			else {
				//说明在本地服务器上有该文件块,选择排序选出两块
				for (int k = 0; k < 2; k++) {
					for (int m = k + 1; m < 5; m++) {
						if (l[k] > l[m]) {
							int temp = 0;
							temp = l[k];
							l[k] = l[m];
							l[m] = temp;
						}
					}
				}
				lix[i][j] = l[1];
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
	////为了计算方便 先对服务器的网络延迟情况进行一次手动输入
	//cout << "手动输入延迟情况" << endl;
	//for (int i = 0; i < 5; i++) {
	//	for (int j = 0; j < 5; j++) {
	//		cin >> lij[i][j];
	//	}
	//}
	////为了计算方便 先对服务器上的文件缓存情况进行一次手动输入
	//cout << "手动输入初始缓存情况" << endl;
	//for (int i = 0; i < 5; i++) {
	//	for (int j = 0; j < 6; j++) {
	//		cin >> lambda[i][j];
	//	}
	//}
	////为了计算方便 先对服务器上的文件热度情况进行一次手动输入
	//cout << "手动输入初始热度情况" << endl;
	//for (int i = 0; i < 5; i++) {
	//	for (int j = 0; j < 6; j++) {
	//		cin >> pix[i][j];
	//	}
	//}


	//这里把数组直接输入，后期针对不同情况改就好，一次次输入有些麻烦

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
