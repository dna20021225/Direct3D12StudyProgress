#pragma once

#include <Windows.h>

class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	void initialize();
	void input();

	bool isPressed(BYTE key) const;
	bool isReleased(BYTE key) const;
	bool isDown(BYTE key) const;
	bool isUp(BYTE key) const;

private:
	static constexpr int KeyNum = 256;

	SHORT m_keyStates[KeyNum];
	SHORT m_keyOldStates[KeyNum];
};