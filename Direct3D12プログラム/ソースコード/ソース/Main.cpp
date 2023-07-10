#include<Windows.h>
#include<memory>

#include"Game.h"

// �E�B���h�E�v���V�[�W���̃v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace {
	// �E�B���h�E�̃N���X���ƃ^�C�g��
	const wchar_t* WinClassName = L"SampleGame";
	const wchar_t* WinGameName = L"Game";

	// �E�B���h�E�̕��ƍ���
	constexpr UINT WinWidth = 640;
	constexpr UINT WinHeight = 480;

	// �Q�[�� 
	std::unique_ptr<Game> game;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	// �E�B���h�E�N���X�\���̂̐ݒ�
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

	// �E�B���h�E�N���X�̓o�^
	if (!RegisterClassExW(&wc)) {
		return 1;
	}

	// �E�B���h�E�𐶐�
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

	// �Q�[���̐����Ə�����
	game = std::make_unique<Game>();
	game->initialize(hwnd, WinWidth, WinHeight);

	// �E�B���h�E��\��
	ShowWindow(hwnd, SW_SHOWNORMAL);

	// ���b�Z�[�W���[�v
	MSG msg = {};
	while (true) {
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		if (msg.message == WM_QUIT)break;

		// �Q�[�����[�v
		if (!game->loop()) {
			DestroyWindow(hwnd);
		}
	}
	// �E�B���h�E�̓o�^����
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	return 0;
}

// �E�B���h�E�v���V�[�W���̒�`
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