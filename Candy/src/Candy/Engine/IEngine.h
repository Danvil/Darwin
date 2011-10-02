#pragma once

namespace Candy
{
	/** Keyboard modifiers are additionally pressed keys like Shift, Ctrl, ... */
	enum KeyboardModifiers {
		NoModifier			= 0x00000000,
		ShiftModifier		= 0x02000000,
		ControlModifier		= 0x04000000,
		AltModifier			= 0x08000000,
		MetaModifier		= 0x10000000,
		KeypadModifier		= 0x20000000,
		GroupSwitchModifier	= 0x40000000
	};

	/** Specifies a mouse button */
	enum MouseButton {
		NoButton			= 0,
		LeftButton			= 1,
		RightButton			= 2,
		MidButton			= 4,
		XButton1			= 8,
		XButton2			= 16
	};

	class IEngine
	{
	public:
		virtual  ~IEngine() {}

		virtual void Init() = 0;

		virtual void Reshape(int width, int height) = 0;

		virtual void Render() = 0;

		virtual void Update(float dt) = 0;

		virtual void OnKeyPressed(KeyboardModifiers mod, int key) = 0;

		virtual void OnKeyReleased(KeyboardModifiers mod, int key) = 0;

		virtual void OnMousePressed(KeyboardModifiers mod, MouseButton button, int x, int y) = 0;

		virtual void OnMouseReleased(KeyboardModifiers mod, MouseButton button, int x, int y) = 0;

		virtual void OnMouseMove(KeyboardModifiers mod, MouseButton buttons, int x, int y, int dx, int dy) = 0;

		virtual void OnWheel(KeyboardModifiers mod, int delta) = 0;

	};
}
