#include "../../BioModels.h"
#include "../../OGLRender.cpp"

#include <stdio.h>
#include <chrono>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_xlib.h>

#include <X11/Xlib.h>

Display* display;
Window win;

inline void UpdateInputKey(Input& input, u32 keyCode, bool isDown)
{
	ASSERT(keyCode >= INPUT_KEY_CODE_Q && keyCode < INPUT_KEY_COUNT);

	input.inputKeys[keyCode].wasDown = input.inputKeys[keyCode].isDown;
	input.inputKeys[keyCode].isDown = isDown;
}

int main()
{
	display = XOpenDisplay(0);
	if (display == 0)
	{
		ASSERT(0);
	}

	Window root = DefaultRootWindow(display);
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

	XVisualInfo* vi = glXChooseVisual(display, 0, att);
	if (vi == 0)
	{
		ASSERT(0);
	}

	XSetWindowAttributes winAttribs;
	winAttribs.colormap = XCreateColormap(display, root, vi->visual, AllocNone);
	winAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;

	win = XCreateWindow(
		display,
		root,
		0,
		0,
		1366,
		768,
		0,
		vi->depth,
		InputOutput,
		vi->visual,
		CWColormap | CWEventMask,
		&winAttribs
	);
	ASSERT(win);

	XMapWindow(display, win);
	XStoreName(display, win, "BioModels");
	GLXContext glc = glXCreateContext(display, vi, 0, GL_TRUE);
	ASSERT(glc);

	glXMakeCurrent(display, win, glc);
	Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, win, &wmDeleteWindow, 1);

	RenderInit();

	SetupImGui();
	ImGui_ImplXlib_Init(display, win);
	ImGui_ImplOpenGL3_Init();

	Input input;
	bool running = true;

	float deltaTime = 0.0f;
	while (running)
	{
		auto frameBeginTime = std::chrono::steady_clock::now();

		while (XPending(display))
		{
			XEvent event;
			XNextEvent(display, &event);
			ImGui_ImplXlib_ProcessEvent(&event);

			switch (event.type)
			{
			case KeyRelease:
			case KeyPress:
			{
				KeySym keysysm = XLookupKeysym(&event.xkey, 0);
				bool isDown = event.type == KeyPress;

				if (keysysm == XK_q)
				{
					UpdateInputKey(input, INPUT_KEY_CODE_Q, isDown);
				}
				if (keysysm == XK_e)
				{
					UpdateInputKey(input, INPUT_KEY_CODE_E, isDown);
				}
				if (keysysm == XK_w)
				{
					UpdateInputKey(input, INPUT_KEY_CODE_W, isDown);
				}
				if (keysysm == XK_s)
				{
					UpdateInputKey(input, INPUT_KEY_CODE_S, isDown);
				}
				if (keysysm == XK_a)
				{
					UpdateInputKey(input, INPUT_KEY_CODE_A, isDown);
				}
				if (keysysm == XK_d)
				{
					UpdateInputKey(input, INPUT_KEY_CODE_D, isDown);
				}

				break;
			}
			case ClientMessage:
			{
				Atom wmProtocol = XInternAtom(display, "WM_PROTOCOLS", False);
				if (event.xclient.message_type == wmProtocol && event.xclient.data.l[0] == wmDeleteWindow)
				{
					running = false;
					break;
				}
			}
			}
		}

		XWindowAttributes attrs;
		XGetWindowAttributes(display, win, &attrs);

		if (!Update(input, deltaTime))
		{
			running = false;
		}
		else
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplXlib_NewFrame();

			RenderResize(0, 0, attrs.width, attrs.height);

			Render(deltaTime, attrs.width, attrs.height);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			RenderSwapBuffers();
		}

		auto frameEndTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> frameDuration = frameEndTime - frameBeginTime;
		deltaTime = frameDuration.count();
#if 0
		float msPerFrame = frameDuration.count() * 1000.0f;
		float fps = 1.0f / frameDuration.count();
		LOG("%.04fms/f,   %.04ff/s\n", msPerFrame, fps);
#endif
	}
}