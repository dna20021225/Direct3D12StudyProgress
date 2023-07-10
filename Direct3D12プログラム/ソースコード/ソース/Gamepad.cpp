#include "Gamepad.h"

Gamepad::Gamepad(DWORD id)
	:m_padID(id)
{
	initialize();
}

Gamepad::~Gamepad()
{

}

void Gamepad::initialize()
{
	memset(m_buttonStates, false, sizeof(bool) * ButtonNum);
	memset(m_buttonOldStates, false, sizeof(bool) * ButtonNum);
	m_leftTrigger = 0;
	m_rightTrigger = 0;
	m_leftStick = ZeroVec;
	m_rightStick = ZeroVec;
}

void Gamepad::input()
{
	memcpy(m_buttonOldStates, m_buttonStates, sizeof(bool) * ButtonNum);

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	DWORD result = XInputGetState(m_padID, &state);
	if (result != ERROR_SUCCESS)
	{
		initialize();
		return;
	}

	for (int i = 0; i < ButtonNum; ++i)
	{
		m_buttonStates[i] = state.Gamepad.wButtons & ButtonList[i];
	}

	m_leftTrigger = state.Gamepad.bLeftTrigger;
	m_rightTrigger = state.Gamepad.bRightTrigger;
	m_leftStick = calcStickVector(static_cast<float>(state.Gamepad.sThumbLX),
		-1.0f * state.Gamepad.sThumbLY, StickMinLength, StickMaxLength);
	m_rightStick = calcStickVector(static_cast<float>(state.Gamepad.sThumbRX),
		-1.0f * state.Gamepad.sThumbRY, StickMinLength, StickMaxLength);
}

bool Gamepad::buttonIndex(int button, int& index)const
{
	for (index = 0; index < ButtonNum; ++index)
	{
		if (button == ButtonList[index])return true;
	}
	return false;
}

bool Gamepad::isPressed(int button)const
{
	int index = 0;
	if (!buttonIndex(button, index))return false;

	return m_buttonStates[index] && !m_buttonOldStates[index];
}

bool Gamepad::isReleased(int button)const
{
	int index = 0;
	if (!buttonIndex(button, index))return false;

	return !m_buttonStates[index] && m_buttonOldStates[index];
}

bool Gamepad::isDown(int button)const
{
	int index = 0;
	if (!buttonIndex(button, index))return false;

	return m_buttonStates[index];
}

bool Gamepad::isUp(int button)const
{
	int index = 0;
	if (!buttonIndex(button, index))return false;

	return !m_buttonStates[index];
}

XMFLOAT2 Gamepad::calcStickVector(float x, float y, float lenMin, float lenMax)const
{
	XMFLOAT2 s(x, y);

	float len = length(s);
	if (len <= lenMin)return ZeroVec;

	len = (len >= lenMax) ? lenMax : len;
	return normalize(s) * ((len - lenMin) / (lenMax - lenMin));
}