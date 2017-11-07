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
using namespace std;

class Solver {
public:
	int turn;
	random_device rnd;
	vector<pair<int, int>> puyo;
	unsigned long long board[8];
	Solver();
	~Solver();
	int beamSearch();
	void init();
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

void Solver::init() {
	puyo.clear();
}

int Solver::beamSearch() {
	vector<pair<int, int>> pre = puyo;
	int next = 72 - 2 * pre.size();
	for(int i = 0; i < next; i++) puyo.push_back(make_pair(rnd() % 4 + 1, rnd() % 4 + 1));
	return rnd() % 24;
}

void Solver::solve(int a, int b, int c, int d) {
	puyo.push_back(make_pair(c, d));
	int num = 5;
	vector<int> ans(24);
	for(int i = 0; i < 5; i++) {
		ans[beamSearch()]++;
		cout << rnd() << endl;
	}
	int id = 0;
	for(int i = 1; i < 24; i++) if(ans[id] < ans[i]) id = i;
	auto pr = numToMove(id);
	cout << "turn " << pr.first << ", x " << pr.second << endl;
	turn++;
};
