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
	return lhs.score > rhs.score;
}

int countChain(Node &node, bool mode);
void stack(Node &node, int act, pair<int, int> puyo);
void printBoard(const Node &node);

class Solver {
public:
	int turn;
	random_device rnd;
	Node now;
	vector<pair<int, int>> puyo;
	unsigned long long board[8];
	Solver();
	~Solver();
	int beamSearch(int);
	void init(int, int, int, int);
	void solve(int, int, int, int);
};

// 0 無回転
// first
// second
//
// 1 逆
// second
// first
//
// 2 右回転
// second first
//
// 3 左回転
// first second
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
	puyo.clear();
	puyo.push_back(make_pair(a - 1, b - 1));
	puyo.push_back(make_pair(c - 1, d - 1));

	for(int i = 0; i < 6; i++) now.col[i] = now.height[i] = 0;
	now.from = -1;
	now.score = 0;
	now.act = 0;

	int num = 5;
	vector<int> ans(24);
	for(int i = 0; i < num; i++) {
		int search = beamSearch(0);
		cout << search << endl;
		ans[search]++;
	}
	int id = 0;
	for(int i = 1; i < 24; i++) if(ans[id] < ans[i]) id = i;

	stack(now, id, puyo[0]);

	auto pr = numToMove(id);
	if(pr.first == 0) {
		pr.second -= 2;
	} else if(pr.first == 1) {
		pr.first = 2;
		pr.second -= 2;
	} else if(pr.first == 2) {
		pr.first = 1;
		pr.second -= 2;
	} else if(pr.first == 3) {
		pr.first = 3;
		pr.second -= 1;
	}
	printBoard(now);
	cout << "turn " << pr.first << " times, move " << pr.second << endl;
	turn++;
}

int bitToColor(unsigned int col, int height, int y) {
	if(y >= height) return 4;
	return (col >> (y * 2)) & 0b11;
}

void printBoard(const Node &node) {
	for(int i = 11; i >= 0; i--) {
		for(int j = 0; j < 6; j++) {
			int a = bitToColor(node.col[j], node.height[j], i);
			if(a < 4) cout << a << " ";
			else cout << "- ";
		}
		cout << endl;
	}
}

int countChain(Node &node, bool mode) {
	int ret = 0;
	bool checked[12][6] = {0};
	bool vanish[12][6] = {0};
	int hx[4] = {1, -1, 0, 0}, hy[4] = {0, 0, 1, -1};
	vector<pair<int, int>> same;
	function<void (int, int, int)> dfs;
	dfs = [&dfs, &checked, &same, &hx, &hy, &node](int y, int x, int color) -> void {
		if(color < 0 || color >= 4 || y < 0 || y >= 12 || x < 0 || x >= 6 || checked[y][x] || bitToColor(node.col[x], node.height[x], y) != color) return;
		checked[y][x] = true;
		same.push_back(make_pair(y, x));
		for(int k = 0; k < 4; k++) dfs(y + hy[k], x + hx[k], color);
	};
	bool chain;
	do {
		int vanishNum = 0;
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
					vanishNum += same.size();
					for(int k = 0; k < same.size(); k++) vanish[same[k].first][same[k].second] = true;
				}
			}
		}
		for(int i = 11; i >= 0; i--) {
			for(int j = 0; j < 6; j++) {
				if(vanish[i][j]) {
					chain = true;
					node.height[j]--;
					if(i > 0) node.col[j] = ((node.col[j] >> (2 * i + 2)) << (2 * i)) | ((node.col[j] << (32 - i * 2)) >> (32 - i * 2));
					else node.col[j] >>= 2;
				}
			}
		}
		if(chain) {
			if(mode) {
				if(vanishNum <= 5) ret++;
			} else {
				ret++;
			}
		}
	} while(chain);
	return ret;
}

int valuate(const Node &node) {
	int ret = 0;
	bool checked[12][6] = {0};
	int hx[4] = {1, -1, 0, 0}, hy[4] = {0, 0, 1, -1};
	vector<pair<int, int>> same;
	function<void (int, int, int)> dfs;
	dfs = [&dfs, &checked, &same, &hx, &hy, &node](int y, int x, int color) -> void {
		if(color < 0 || color >= 4 || y < 0 || y >= 12 || x < 0 || x >= 6 || checked[y][x] || bitToColor(node.col[x], node.height[x], y) != color) return;
		checked[y][x] = true;
		same.push_back(make_pair(y, x));
		for(int k = 0; k < 4; k++) dfs(y + hy[k], x + hx[k], color);
	};
	for(int i = 11; i >= 0; i--) for(int j = 0; j < 6; j++) {
		checked[i][j] = false;
	}
	for(int i = 11; i >= 0; i--) {
		for(int j = 0; j < 6; j++) {
			if(checked[i][j]) continue;
			same.clear();
			int color = bitToColor(node.col[j], node.height[j], i);
			if(color == 4) continue;
			dfs(i, j, color);
			ret += same.size() * same.size();
		}
	}
	ret += ((node.height[0] < 11) ? node.height[0] * 2 : 20);
	ret += ((node.height[1] < 11) ? node.height[1] * 1 : 10);
	ret += ((node.height[4] < 11) ? node.height[4] * 1 : 10);
	ret += ((node.height[5] < 11) ? node.height[5] * 2 : 20);
	int maxChain = 0;
	for(int i = 0; i < 24; i++) {
		Node chain;
		for(int j = 0; j < 6; j++) {
			chain.col[j] = node.col[j];
			chain.height[j] = node.height[j];
		}
		int x = i / 6;
		int color = i % 4;
		chain.col[x] |= color << (chain.height[x]++ * 2);
		maxChain = max(maxChain, countChain(chain, true));
	}
	ret += maxChain * 10;
	return ret;
}

void stack(Node &node, int act, pair<int, int> puyo) {
	int x = act % 6;
	int turn = act / 6;
	if(turn == 0) {
		node.col[x] |= puyo.second << (node.height[x]++ * 2);
		node.col[x] |= puyo.first << (node.height[x]++ * 2);
	} else if(turn == 1) {
		node.col[x] |= puyo.first << (node.height[x]++ * 2);
		node.col[x] |= puyo.second << (node.height[x]++ * 2);
	} else if(turn == 2) {
		node.col[x] |= puyo.second << (node.height[x]++ * 2);
		x++;
		node.col[x] |= puyo.first << (node.height[x]++ * 2);
	} else {
		node.col[x] |= puyo.first << (node.height[x]++ * 2);
		x++;
		node.col[x] |= puyo.second << (node.height[x]++ * 2);
	}
}

int Solver::beamSearch(int turn) {
	vector<pair<int, int>> pre = puyo;
	int next = 36 - pre.size();
	for(int i = 0; i < next; i++) pre.push_back(make_pair(rnd() % 4, rnd() % 4));
	vector<vector<Node>> beam(next);
	vector<Node> chainNodes(next);
	for(int i = 0; i < next; i++) chainNodes[i].score = -1;

	for(int i = 0; i < 24; i++) {
		if(i >= 12 && i % 6 == 5) continue;
		if(pre[turn].first == pre[turn].second && (i / 6 == 1 || i / 6 == 3)) continue;
		Node node;
		for(int j = 0; j < 6; j++) {
			node.col[j] = now.col[j];
			node.height[j] = now.height[j];
		}
		node.from = -1;
		node.act = i;
		stack(node, i, pre[turn]);
		int chainNum = countChain(node, false);
		if(chainNum > 0) {
			node.score = chainNum;
			if(chainNodes[0].score < chainNum) chainNodes[0] = node;
			continue;
		}
		node.score = valuate(node);
		beam[0].push_back(node);
	}
	int cnt = 0, beamLength = 50;
	for(int i = 0; i < next - 1; i++) {
		if(beam[i].size() > 0) sort(beam[i].begin(), beam[i].end());
		if(beam[i].size() > beamLength) beam[i].resize(beamLength);
		//cout << i << " " << beam[i].size() << endl;
		for(int k = 0; k < beamLength && k < beam[i].size(); k++) {
			for(int j = 0; j < 24; j++) {
				if(j >= 12 && j % 6 == 5) continue;
				if(pre[turn + i + 1].first == pre[turn + i + 1].second && (i / 6 == 1 || i / 6 == 3)) continue;
				Node node;
				for(int l = 0; l < 6; l++) {
					node.col[l] = beam[i][k].col[l];
					node.height[l] = beam[i][k].height[l];
				}
				node.from = k;
				node.act = j;
				stack(node, j, pre[turn + i + 1]);
				int chainNum = countChain(node, false);
				if(chainNum > 0) {
					node.score = chainNum;
					if(chainNodes[i + 1].score < chainNum) chainNodes[i + 1] = node;
					continue;
				}
				if(node.height[0] > 11 || node.height[1] > 11 || node.height[2] > 11 || node.height[3] > 11 || node.height[4] > 11 || node.height[5] > 11) {
					continue;
				}
				node.score = valuate(node);
				beam[i + 1].push_back(node);
			}
		}
	}

	int id = 0;
	if(turn < 10) id = 1;
	for(int i = 1; i < next; i++) if(chainNodes[id].score < chainNodes[i].score) id = i;
	// cout << id << " " << chainNodes[id].score << endl;
	if(chainNodes[id].score < 0) id = 0;
	int from = chainNodes[id].from, act = chainNodes[id].act;
	if(id == 0) {
		return chainNodes[0].act;
	}
	/*
	for(int j = 11; j >= 0; j--) {
		for(int k = 0; k < 6; k++) cout << bitToColor(chainNodes[id].col[k], chainNodes[id].height[k], j) << " ";
		cout << endl;
	}
	*/

	vector<int> froms;
	id--;
	for(; id > 0; id--) {
		froms.push_back(from);
		from = beam[id][from].from;
	}
	froms.push_back(from);
	reverse(froms.begin(), froms.end());
	/*
	for(int i = 0; i < froms.size(); i++) {
		cout << "turn " << i << endl;
		for(int j = 11; j >= 0; j--) {
			for(int k = 0; k < 6; k++) cout << bitToColor(beam[i][froms[i]].col[k], beam[i][froms[i]].height[k], j) << " ";
			cout << endl;
		}
	}
	cout << puyo[froms.size()].first << " " << puyo[froms.size()].second << endl;
	cout << act << endl;
	for(int j = 11; j >= 0; j--) {
		for(int k = 0; k < 6; k++) cout << bitToColor(chainNodes[froms.size()].col[k], chainNodes[froms.size()].height[k], j) << " ";
		cout << endl;
	}
	*/
	return beam[0][from].act;
}

void Solver::solve(int a, int b, int c, int d) {
	cout << "solve" << endl;
	puyo.push_back(make_pair(c - 1, d - 1));
	int num = 5;
	vector<int> ans(24);
	for(int i = 0; i < num; i++) {
		int search = beamSearch(turn);
		cout << search << endl;
		ans[search]++;
	}
	int id = 0;
	for(int i = 1; i < 24; i++) if(ans[id] < ans[i]) id = i;
	stack(now, id, puyo[turn]);
	auto pr = numToMove(id);
	if(pr.first == 0) {
		pr.second -= 2;
	} else if(pr.first == 1) {
		pr.first = 2;
		pr.second -= 2;
	} else if(pr.first == 2) {
		pr.first = 1;
		pr.second -= 2;
	} else if(pr.first == 3) {
		pr.first = 3;
		pr.second -= 1;
	}
	printBoard(now);
	cout << "turn " << pr.first << ", x " << pr.second << endl;
	turn++;
};


void test() {
	random_device rnd;
	vector<pair<int, int>> chainTest(100);
	for(int i = 0; i < 100; i++) {
		Solver solver;
		vector<pair<int, int>> p(40);
		p[0].first = rnd() % 3;
		p[0].second = rnd() % 3;
		p[1].first = rnd() % 3;
		p[1].second = rnd() % 3;
		for(int j = 2; j < 40; j++) {
			p[j].first = rnd() % 4;
			p[j].second = rnd() % 4;
		}
		solver.init(p[0].first, p[0].second, p[1].first, p[1].second);
		for(int j = 1; j < 39; j++) {
			solver.solve(p[j].first, p[j].second, p[j + 1].first, p[j + 1].second);
			int chainNum = countChain(solver.now, false);
			if(chainNum > 0) {
				chainTest[i].first = chainNum;
				chainTest[i].second = solver.turn;
				break;
			}
		}
	}
	for(int i = 0; i < 100; i++) {
		cout << chainTest[i].first << " " << chainTest[i].second << endl;
	}
}
