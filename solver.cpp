#include<iostream>
#include<fstream>
#include<stdio.h>
#include<string>
#include<vector>
#include<map>
#include<math.h>
#include<algorithm>
#include<iomanip>
#include<set>
#include<random>
#include<functional>
using namespace std;


struct Node {
	unsigned int col[6];
	int height[6];
	int from, score, act;
};

bool operator<(const Node &lhs, const Node &rhs) {
	return lhs.score < rhs.score;
}

int countChain(Node &node);

class Solver {
public:
	int turn;
	random_device rnd;
	vector<pair<int, int>> puyo;
	unsigned long long board[8];
	Solver();
	~Solver();
	int beamSearch();
	void init(int, int, int, int);
	void solve(int, int, int, int);
};

int moveToNum(int rot, int x) {
	return rot * 6 + x;
}

pair<int, int> numToMove(int x) {
	return make_pair(x / 6, x % 6);
}

Solver::Solver() {
	turn = 0;
	for(int i = 0; i < 8; i++) board[i] = 0;
}
Solver::~Solver() {

}

void Solver::init(int a, int b, int c, int d) {
	Node node;
	for(int i = 0; i < 6; i++) {
		node.col[i] = 0;
		node.height[i] = 1;
	}
	node.col[4] = 1;
	node.col[5] = 1;
	cout << countChain(node) << endl;
	for(int i = 0; i < 6; i++) cout << node.col[i] << endl;
	for(int i = 0; i < 6; i++) cout << node.height[i] << endl;

	exit(0);

	puyo.clear();
	puyo.push_back(make_pair(a - 1, b - 1));
	puyo.push_back(make_pair(c - 1, d - 1));
	int num = 23;
	vector<int> ans(24);
	for(int i = 0; i < num; i++) {
		ans[beamSearch()]++;
	}
	int id = 0;
	for(int i = 1; i < 24; i++) if(ans[id] < ans[i]) id = i;
	auto pr = numToMove(id);
	cout << "turn " << pr.first << ", x " << pr.second << endl;
	turn++;
}

int bitToColor(unsigned int col, int height, int y) {
	if(y >= height) return 4;
	return (col >> (y * 2)) & 0b11;
}

int countChain(Node &node) {
	int ret = 0;
	bool checked[12][6] = {0};
	bool vanish[12][6] = {0};
	int hx[4] = {1, -1, 0, 0}, hy[4] = {0, 0, 1, -1};
	vector<pair<int, int>> same;
	function<void (int, int, int)> dfs;
	dfs = [&dfs, &checked, &same, &hx, &hy](int y, int x, int color) -> void {
		if(color < 0 || color >= 4 || y < 0 || y >= 12 || x < 0 || x >= 6 || checked[y][x]) return;
		checked[y][x] = true;
		same.push_back(make_pair(y, x));
		for(int k = 0; k < 4; k++) dfs(y + hy[k], x + hx[k], color);
	};
	bool chain;
	do {
		chain = false;
		for(int i = 11; i >= 0; i--) for(int j = 0; j < 6; j++) {
			checked[i][j] = false;
			vanish[i][j] = false;
		}
		for(int i = 11; i >= 0; i--) {
			for(int j = 0; j < 6; j++) {
				if(checked[i][j]) continue;
				same.clear();
				int color = bitToColor(node.col[j], node.height[j], i);
				if(color == 4) continue;
				dfs(i, j, color);
				if(same.size() >= 4) {
					for(int k = 0; k < same.size(); k++) vanish[same[k].first][same[k].second] = true;
				}
			}
		}
		for(int i = 11; i >= 0; i--) {
			for(int j = 0; j < 6; j++) {
				if(vanish[i][j]) {
					chain = true;
					node.height[j]--;
					node.col[j] = ((node.col[j] >> (2 * i + 2)) << (2 * i)) | ((node.col[j] << (32 - i * 2)) >> (32 - i * 2));
				}
			}
		}
		if(chain) ret++;
	} while(chain);
	return ret;
}

int Solver::beamSearch() {
	vector<pair<int, int>> pre = puyo;
	int next = 72 - 2 * pre.size();
	for(int i = 0; i < next; i++) puyo.push_back(make_pair(rnd() % 4, rnd() % 4));





	return rnd() % 24;
}

void Solver::solve(int a, int b, int c, int d) {
	puyo.push_back(make_pair(c - 1, d - 1));
	int num = 23;
	vector<int> ans(24);
	for(int i = 0; i < num; i++) {
		ans[beamSearch()]++;
	}
	int id = 0;
	for(int i = 1; i < 24; i++) if(ans[id] < ans[i]) id = i;
	auto pr = numToMove(id);
	cout << "turn " << pr.first << ", x " << pr.second << endl;
	turn++;
};
