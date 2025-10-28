#pragma once

class IInputHandler {
public:
	IInputHandler() {}
	virtual ~IInputHandler() {}
	bool virtual isKeyPressed(int keyCode) = 0;
	bool virtual isMouseButtonPressed(int button) = 0;
	void virtual getMousePos(double& x, double& y) = 0;

};