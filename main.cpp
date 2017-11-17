#include<iostream>
#include<vector>
#include<string>
#include<windows.h>
#include "solver.cpp"
using namespace std;

struct Color {
	int r, g, b;
	struct Color& operator+=(const Color& rhs) {r += rhs.r; g += rhs.g; b += rhs.b; return *this; }
	struct Color& operator/=(const int n) {r /= n; g /= n; b /= n; return *this; }
	int squareDist(const Color& rhs) {return (r - rhs.r) * (r - rhs.r) + (g - rhs.g) * (g - rhs.g) + (b - rhs.b) * (b - rhs.b);}
};

int rgbToH(Color col) {
	if(col.r == col.b && col.b == col.g) return -1;
	if(col.b <= col.g && col.b <= col.r) return (60 * (col.g - col.r) / (max(col.g, col.r) - col.b) + 60);
	if(col.r <= col.g && col.r <= col.b) return (60 * (col.b - col.g) / (max(col.b, col.g) - col.r) + 180);
	if(col.g <= col.r && col.g <= col.b) return (60 * (col.r - col.b) / (max(col.r, col.b) - col.g) + 300);
	return -1;
}

int rgbToV(Color col) {
	return max(max(col.r, col.g), col.b);
}

int rgbToS(Color col) {
	return max(max(col.r, col.g), col.b) - min(min(col.r, col.g), col.b);
}

// 32bit(0x[00rrggbb])からRGBへの変換
Color refToColor(COLORREF c) {
	Color ret;
	ret.r = c & 255;
	ret.g = (c >> 8) & 255;
	ret.b = (c >> 16) & 255;
	return ret;
}

BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM IParam) {
	char title[256];
	GetWindowText(hWnd, title, sizeof(title));
	string str = title;
	// キャプチャのプロセス名
	if(str.substr(0, 6) == "AmaRec") {
		cout << "find" << endl;
		if(IsWindow) *((HWND*)IParam) = hWnd;
	}
	return true;
}

// RGBからぷよの色への変換
// 0: なし, 1: 赤, 2: 緑, 3: 青, 4: 黄
int colorToPuyo(Color col) {
	static Color puyo[4] = {Color{180, 0, 0}, Color{50, 170, 50}, Color{10, 25, 140}, Color{145, 116, 25}};
	int ret = 0;
	for(int i = 1; i < 4; i++) if(col.squareDist(puyo[ret]) > col.squareDist(puyo[i])) ret = i;
	return ret + 1;
}

// Sからぷよの色への変換
// 0: なし, 1: 赤, 2: 緑, 3: 青, 4: 黄, 5: 背景/不適
int hToPuyo(int h) {
	if(h < 10 || 340 <= h) return 1;
	if(40 <= h && h < 80) return 4;
	if(80 <= h && h < 140) return 2;
	if(220 <= h && h < 280) return 3;
	return 5;
}

vector<Color> getPuyoColor(HWND handle) {
	vector<Color> ret(4);
	HDC hDC = GetDC(handle);
	ret[0] = refToColor(GetPixel(hDC, 288, 119));
	ret[0] += refToColor(GetPixel(hDC, 300, 119));
	ret[0] += refToColor(GetPixel(hDC, 300, 131));
	ret[0] += refToColor(GetPixel(hDC, 288, 131));
	ret[0] += refToColor(GetPixel(hDC, 294, 125));
	ret[0] /= 5;
	ret[1] = refToColor(GetPixel(hDC, 288, 144));
	ret[1] += refToColor(GetPixel(hDC, 300, 144));
	ret[1] += refToColor(GetPixel(hDC, 300, 156));
	ret[1] += refToColor(GetPixel(hDC, 288, 156));
	ret[1] += refToColor(GetPixel(hDC, 294, 150));
	ret[1] /= 5;
	ret[2] = refToColor(GetPixel(hDC, 308, 173));
	ret[2] += refToColor(GetPixel(hDC, 318, 173));
	ret[2] += refToColor(GetPixel(hDC, 318, 182));
	ret[2] += refToColor(GetPixel(hDC, 308, 182));
	ret[2] += refToColor(GetPixel(hDC, 313, 179));
	ret[2] /= 5;
	ret[3] = refToColor(GetPixel(hDC, 308, 195));
	ret[3] += refToColor(GetPixel(hDC, 318, 195));
	ret[3] += refToColor(GetPixel(hDC, 318, 202));
	ret[3] += refToColor(GetPixel(hDC, 308, 202));
	ret[3] += refToColor(GetPixel(hDC, 313, 201));
	ret[3] /= 5;
	ReleaseDC(handle, hDC);
	return ret;
}

int main() {
	HWND handle;
	EnumWindows(EnumWndProc, (LPARAM)&handle);
	Solver solver;
	bool init = true;

	// debug

	while(true) {
		if(IsWindow(handle)) {
			vector<Color> cols = getPuyoColor(handle);
			vector<int> puyo(4);
			for(int i = 0; i < 4; i++) puyo[i] = hToPuyo(rgbToH(cols[i]));

			int cnt = 0;
			for(int i = 0; i < 4; i++) if(puyo[i] == 5) cnt++;
			// 背景色を多数検知したとき.25秒後から.30秒後のぷよ色から色を得る
			if(cnt >= 2) {
				Sleep(250);
				vector<vector<int>> vote(4);
				for(int i = 0; i < 4; i++) vote[i].resize(6);
				for(int i = 0; i < 5; i++) {
					cols = getPuyoColor(handle);
					for(int j = 0; j < 4; j++) vote[j][hToPuyo(rgbToH(cols[j]))]++;
					Sleep(10);
				}
				vector<int> col(4);
				// 多数決により色を判定
				for(int i = 0; i < 4; i++) {
					int id = 0;
					for(int j = 1; j < 6; j++) if(vote[i][id] < vote[i][j]) id = j;
					cout << id << " ";
					col[i] = id;
				}
				cout << endl;
				solver.solve(col[0], col[1], col[2], col[3]);
			}
			if(init) {
				init = false;
				vector<vector<int>> vote(4);
				for(int i = 0; i < 4; i++) vote[i].resize(6);
				for(int i = 0; i < 5; i++) {
					cols = getPuyoColor(handle);
					for(int j = 0; j < 4; j++) vote[j][hToPuyo(rgbToH(cols[j]))]++;
					Sleep(10);
				}
				vector<int> col(4);
				// 多数決により色を判定
				for(int i = 0; i < 4; i++) {
					int id = 0;
					for(int j = 1; j < 6; j++) if(vote[i][id] < vote[i][j]) id = j;
					cout << id << " ";
					col[i] = id;
				}
				solver.init(col[0], col[1], col[2], col[3]);
			}
			// 赤 [355-5]
			// 青 [230-250]
			// 緑 [85-120]
			// 黄 [40-55]
			// 背景 [15-33]
			Sleep(100);
		} else break;
	}
	return 0;
}
