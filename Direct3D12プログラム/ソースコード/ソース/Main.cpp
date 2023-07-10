#include<Windows.h>
#include<memory>

#include"Game.h"

// ウィンドウプロシージャのプロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace {
	// ウィンドウのクラス命とタイトル
	const wchar_t* WinClassName = L"SampleGame";
	const wchar_t* WinGameName = L"Game";

	// ウィンドウの幅と高さ
	constexpr UINT WinWidth = 640;
	constexpr UINT WinHeight = 480;

	// ゲーム 
	std::unique_ptr<Game> game;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	// ウィンドウクラス構造体の設定
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIconW(nullptr, L"IDI_ICON");
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszClassName = WinClassName;
	wc.hIconSm = LoadIconW(nullptr, L"IDI_ICON");

	// ウィンドウクラスの登録
	if (!RegisterClassExW(&wc)) {
		return 1;
	}

	// ウィンドウを生成
	RECT rc = {};
	rc.right = static_cast<LONG>(WinWidth);
	rc.bottom = static_cast<LONG>(WinHeight);

	DWORD style = WS_SYSMENU | WS_DLGFRAME | WS_MINIMIZEBOX;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW | style, FALSE);

	HWND hwnd;
	hwnd = CreateWindowExW(0, WinClassName, WinGameName, WS_OVERLAPPED | style, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	if (!hwnd) {
		return 1;
	}

	// ゲームの生成と初期化
	game = std::make_unique<Game>();
	game->initialize(hwnd, WinWidth, WinHeight);

	// ウィンドウを表示
	ShowWindow(hwnd, SW_SHOWNORMAL);

	// メッセージループ
	MSG msg = {};
	while (true) {
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		if (msg.message == WM_QUIT)break;

		// ゲームループ
		if (!game->loop()) {
			DestroyWindow(hwnd);
		}
	}
	// ウィンドウの登録解除
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	return 0;
}

// ウィンドウプロシージャの定義
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		switch (wparam) {
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			return 0;
		}
		break;
	}
	return DefWindowProcW(hwnd, msg, wparam, lparam);
}