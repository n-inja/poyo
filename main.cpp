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

int main() {
	HWND handle;
	EnumWindows(EnumWndProc, (LPARAM)&handle);
	Solver solver;
	bool init = true;
	while(true) {
		if(IsWindow(handle)) {
			string s;
			cin >> s;
			if(s == "quit") break;
			HDC hDC = GetDC(handle);

			vector<Color> cols(4);
			cols[0] = refToColor(GetPixel(hDC, 288, 119));
			cols[0] += refToColor(GetPixel(hDC, 300, 119));
			cols[0] += refToColor(GetPixel(hDC, 300, 131));
			cols[0] += refToColor(GetPixel(hDC, 288, 131));
			cols[0] += refToColor(GetPixel(hDC, 294, 125));
			cols[0] /= 5;
			cols[1] = refToColor(GetPixel(hDC, 288, 144));
			cols[1] += refToColor(GetPixel(hDC, 300, 144));
			cols[1] += refToColor(GetPixel(hDC, 300, 156));
			cols[1] += refToColor(GetPixel(hDC, 288, 156));
			cols[1] += refToColor(GetPixel(hDC, 294, 150));
			cols[1] /= 5;
			cols[2] = refToColor(GetPixel(hDC, 308, 173));
			cols[2] += refToColor(GetPixel(hDC, 318, 173));
			cols[2] += refToColor(GetPixel(hDC, 318, 182));
			cols[2] += refToColor(GetPixel(hDC, 308, 182));
			cols[2] += refToColor(GetPixel(hDC, 313, 179));
			cols[2] /= 5;
			cols[3] = refToColor(GetPixel(hDC, 308, 195));
			cols[3] += refToColor(GetPixel(hDC, 318, 195));
			cols[3] += refToColor(GetPixel(hDC, 318, 202));
			cols[3] += refToColor(GetPixel(hDC, 308, 202));
			cols[3] += refToColor(GetPixel(hDC, 313, 201));
			cols[3] /= 5;
			vector<int> puyo(4);
			for(int i = 0; i < 4; i++) puyo[i] = colorToPuyo(cols[i]);
			cout << puyo[0] << " " << puyo[1] << ", " << puyo[2] << " " << puyo[3] << endl;

			ReleaseDC(handle, hDC);
			if(init) {
				solver.init(puyo[0], puyo[1], puyo[2], puyo[3]);
				init = false;
			} else solver.solve(puyo[0], puyo[1], puyo[2], puyo[3]);

		} else break;
	}
	return 0;
}
