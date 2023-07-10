#pragma once

#include <Windows.h>
#include"GMath.h"

class Mouse
{
public:
	Mouse();
	~Mouse();

	void initialize();
	void input(HWND hwnd);

	bool isPressed(BYTE key)const;
	bool isReleased(BYTE key)const;
	bool isDown(BYTE key)const;
	bool isUp(BYTE key)const;

	XMFLOAT2 getPos()const { return m_pos; }

private:
	static constexpr int KeyNum = 3;
	static constexpr BYTE VK_keys[KeyNum] = { VK_LBUTTON,VK_RBUTTON,VK_MBUTTON };

	SHORT m_keyStates[KeyNum];
	short m_keyOldStates[KeyNum];

	bool keyIndex(BYTE key, int& index)const;

	XMFLOAT2 m_pos;
};