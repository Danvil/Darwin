#include "GlutHook.h"
#include <Candy/Tools/Timer.h>
#include <GL/freeglut.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
//#define VERBOSE

namespace Candy
{
	namespace GlutHook
	{
		boost::shared_ptr<Candy::IEngine> engine_;
	
		std::string name_ = "Candy";
	
		size_t frame_counter = 0;
	
		Candy::MouseButton lastButton = Candy::NoButton;
	
		Candy::KeyboardModifiers lastModifier = Candy::NoModifier;
	
		int lastX, lastY;

		Danvil::Timer timer;

		Danvil::Timer timer_fps;

		float last_fps_ = 0.0;

		float last_time_update_ = 0.0;

		float last_time_render_ = 0.0;

	}
}

using namespace Candy::GlutHook;

void UpdateTitle()
{
	char* title = new char[name_.length() + 256];
	sprintf(title, "%s --- fps: %6.2f 1/s, render: %6.2f ms, update: %6.2f ms",
		name_.c_str(), last_fps_, last_time_render_, last_time_update_);
	glutSetWindowTitle(title);
	delete[] title;
}

void GlutHook_idle()
{
#ifdef VERBOSE
	std::cout << "GlutHook_idle" << std::endl;
#endif
	try {
		double dt = Candy::GlutHook::timer.tick();
		Danvil::Timer t;
		t.start();
		engine_->Update(float(dt));
		float time =  (float)t.getElapsedTimeInMilliSec();
		if(time > 1000.0f) {
			last_time_update_ = time;
		}
		else {
			last_time_update_ = 0.99f * last_time_update_ + 0.01f * time;
		}
	}
	catch(const std::runtime_error& e) {
		std::cerr << "EXCEPTION " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "EXCEPTION while calling IEngine::Update!" << std::endl;
	}
}

void GlutHook_reshape(int w, int h)
{
#ifdef VERBOSE
	std::cout << "GlutHook_reshape size=" << w << "x" << h << std::endl;
#endif
	try {
		engine_->Reshape(w, h);
	}
	catch(const std::runtime_error& e) {
		std::cerr << "EXCEPTION " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "EXCEPTION while calling IEngine::Reshape!" << std::endl;
	}
	glutPostRedisplay();
}

void GlutHook_display()
{
#ifdef VERBOSE
	std::cout << "GlutHook_display" << std::endl;
#endif
	try {
		double dt;
		if(timer_fps.tick(0.5, &dt)) {
			last_fps_ = (float)(frame_counter) / (float)dt;
			frame_counter = 0;
			UpdateTitle();
		}
		Danvil::Timer t;
		t.start();
		engine_->Render();
		boost::this_thread::sleep(boost::posix_time::milliseconds(2));
		float time =  (float)t.getElapsedTimeInMilliSec();
		if(time > 1000.0f) {
			last_time_render_ = time;
		}
		else {
			last_time_render_ = 0.99f * last_time_render_ + 0.01f * time;
		}
		frame_counter++;
	}
	catch(const std::runtime_error& e) {
		std::cerr << "EXCEPTION " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "EXCEPTION while calling IEngine::Render!" << std::endl;
	}
	glutSwapBuffers();
	glutPostRedisplay();
}

Candy::KeyboardModifiers getModifier()
{
	switch(glutGetModifiers()) {
	case GLUT_ACTIVE_SHIFT:	return Candy::ShiftModifier;
	case GLUT_ACTIVE_CTRL:	return Candy::ControlModifier;
	case GLUT_ACTIVE_ALT:	return Candy::AltModifier;
	default:				return Candy::NoModifier;
	}
}

Candy::MouseButton getMouseButton(int glutButton)
{
	switch(glutButton) {
	case GLUT_LEFT_BUTTON:	return Candy::LeftButton;
	case GLUT_MIDDLE_BUTTON:return Candy::MidButton;
	case GLUT_RIGHT_BUTTON: return Candy::RightButton;
	default: 				return Candy::NoButton;
	};
}

void GlutHook_motion(int x, int y)
{
#ifdef VERBOSE
	std::cout << "GlutHook_motion pos=(" << x << "," << y << ")" << std::endl;
#endif
	int dx = x - lastX;
	int dy = y - lastY;
	lastX = x;
	lastY = y;
	//cout << "motion " << x << " " << y << " " << lastButton << endl;
	engine_->OnMouseMove(lastModifier, lastButton, x, y, dx, dy);
}

void GlutHook_passive_motion(int x, int y)
{
#ifdef VERBOSE
	std::cout << "GlutHook_passive_motion pos=(" << x << "," << y << ")" << std::endl;
#endif
	int dx = x - lastX;
	int dy = y - lastY;
	lastX = x;
	lastY = y;
	//cout << "motion " << x << " " << y << endl;
	engine_->OnMouseMove(lastModifier, Candy::NoButton, x, y, dx, dy);
}

void GlutHook_mouse(int glutButton, int state, int x, int y)
{
#ifdef VERBOSE
	std::cout << "GlutHook_mouse button=" << glutButton << ", state=" << state << ", pos=(" << x << "," << y << ")" << std::endl;
#endif
	//cout << "mouse" << endl;
	lastButton = getMouseButton(glutButton);
	lastModifier = getModifier();
	if(glutButton == 3 || glutButton == 4) {
		if(glutButton == 3) {
			// wheel up
			engine_->OnWheel(lastModifier, +1);
		} else if(glutButton == 4) {
			// wheel down
			engine_->OnWheel(lastModifier, -1);
		}
	} else {
		if( state == GLUT_DOWN ) {
			engine_->OnMousePressed(lastModifier, lastButton, x, y);
		} else if( state == GLUT_UP ) {
			engine_->OnMouseReleased(lastModifier, lastButton, x, y);
		}
	}
}

void GlutHook_keyboard(unsigned char key, int x, int y)
{
#ifdef VERBOSE
	std::cout << "GlutHook_keyboard key='" << int(key) << "' pos=(" << x << "," << y << ")" << std::endl;
#endif
	lastModifier = getModifier();
	engine_->OnKeyPressed(lastModifier, key);
}

namespace Candy
{
	namespace GlutHook
	{
		void Initialize(const std::string& caption, boost::shared_ptr<IEngine> engine)
		{
			if(engine_) {
				throw std::runtime_error("Glut hook already initialized!");
			}
			engine_ = engine;
			name_ = caption;
		}

		void Run(int argc, char** argv)
		{
			glutInit(&argc, argv);
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
			glutCreateWindow(name_.c_str());

			glutDisplayFunc(GlutHook_display);
			glutReshapeFunc(GlutHook_reshape);
			glutIdleFunc(GlutHook_idle);
			glutMotionFunc(GlutHook_motion);
			glutPassiveMotionFunc(GlutHook_passive_motion);
			glutMouseFunc(GlutHook_mouse);
			glutKeyboardFunc(GlutHook_keyboard);

			glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
			timer.start();
			timer_fps.start();
			engine_->Init();
			glutMainLoop();
		}
	}
}
