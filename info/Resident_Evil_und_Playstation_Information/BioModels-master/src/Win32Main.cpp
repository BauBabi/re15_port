#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>

#include "BioModels.h"

#ifdef OGL_RENDER
#include "OGLRender.cpp"
#endif

HWND hWND;
HDC hDC;
WNDCLASSEXA wndCLASSEXA;

static bool gRunning = true;
static float gDeltaTime;
static Input gInput;

struct Win32Dimension
{
	int width;
	int height;
};

static Win32Dimension GetWin32Dimension(HWND window)
{
	RECT clientRect;
	GetClientRect(window, &clientRect);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	return Win32Dimension{ width, height };
}

inline void UpdateInputKey(Input& input, u32 keyCode, bool isDown, bool wasDown)
{
	ASSERT(keyCode >= INPUT_KEY_CODE_Q && keyCode < INPUT_KEY_COUNT);

	gInput.inputKeys[keyCode].isDown = isDown;
	gInput.inputKeys[keyCode].wasDown = wasDown;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	LRESULT result = 0;

	switch (msg)
	{
	case WM_CLOSE:
	{
		LOG("WM_CLOSE\n");
		gRunning = false;
		break;
	}
	case WM_SIZE:
	{
		auto dimension = GetWin32Dimension(window);
		LOG("WM_SIZE, width: %d, height: %d\n", dimension.width, dimension.height);
		RenderResize(0, 0, dimension.width, dimension.height);
		break;
	}
	case WM_ACTIVATEAPP:
	{
		LOG("WM_ACTIVATEAPP\n");
		break;
	}
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_KEYDOWN:
	{
		u32 vkCode = (u32)wParam;
		bool wasDown = (lParam >> 30) & 1;
		bool isDown = ((lParam >> 31) & 1) == 0;
		bool altKeyWasDown = (lParam >> 29) & 1;

		if (altKeyWasDown && vkCode == VK_F4)
		{
			// TODO: Display MessageBox and ask for exit
			gRunning = false;
		}
		if (vkCode == 'Q')
		{
			UpdateInputKey(gInput, INPUT_KEY_CODE_Q, isDown, wasDown);
		}
		if (vkCode == 'E')
		{
			UpdateInputKey(gInput, INPUT_KEY_CODE_E, isDown, wasDown);
		}
		if (vkCode == 'W')
		{
			UpdateInputKey(gInput, INPUT_KEY_CODE_W, isDown, wasDown);
		}
		if (vkCode == 'S')
		{
			UpdateInputKey(gInput, INPUT_KEY_CODE_S, isDown, wasDown);
		}
		if (vkCode == 'A')
		{
			UpdateInputKey(gInput, INPUT_KEY_CODE_A, isDown, wasDown);
		}
		if (vkCode == 'D')
		{
			UpdateInputKey(gInput, INPUT_KEY_CODE_D, isDown, wasDown);
		}
		break;
	}
	default:
	{
		return DefWindowProcA(window, msg, wParam, lParam);
	}
	}

	return result;
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, PSTR cmdline, int cmdshow)
{
#ifdef _DEBUG
	AllocConsole();
	FILE* fconsole;
	freopen_s(&fconsole, "CONOUT$", "w", stdout);
	LOG("BioModels Debug console\n");
#endif

	s64 perfCounterFrequency;

	LARGE_INTEGER perfCounterFrequencyResult;
	QueryPerformanceFrequency(&perfCounterFrequencyResult);
	perfCounterFrequency = perfCounterFrequencyResult.QuadPart;

	wndCLASSEXA = {};
	wndCLASSEXA.cbSize = sizeof(WNDCLASSEX);
	wndCLASSEXA.hInstance = instance;
	wndCLASSEXA.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wndCLASSEXA.lpfnWndProc = WindowCallback;
	wndCLASSEXA.lpszClassName = "BioModelsWindowClass";

	ATOM registerResult = RegisterClassExA(&wndCLASSEXA);
	ASSERT(registerResult);

	hWND = CreateWindowExA(
		0,
		wndCLASSEXA.lpszClassName,
		"BioModels",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1366, 768,
		0, 0, instance, 0
	);

	hDC = GetWindowDC(hWND);

	RenderInit();

	ShowWindow(hWND, SW_SHOW);

	SetupImGui();
	ImGui_ImplWin32_InitForOpenGL(hWND);
	ImGui_ImplOpenGL3_Init();

	LARGE_INTEGER lastFrameTime;
	QueryPerformanceCounter(&lastFrameTime);

	while (gRunning)
	{
		MSG msg;
		while (PeekMessageW(&msg, hWND, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				gRunning = false;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		auto dimension = GetWin32Dimension(hWND);

		if (!Update(gInput, gDeltaTime))
		{
			gRunning = false;
		}
		else
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplWin32_NewFrame();

			Render(gDeltaTime, dimension.width, dimension.height);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			RenderSwapBuffers();
		}

		LARGE_INTEGER endFrameTime;
		QueryPerformanceCounter(&endFrameTime);

		s64 frameTime = endFrameTime.QuadPart - lastFrameTime.QuadPart;
		gDeltaTime = (float)frameTime / (float)perfCounterFrequency;
#if 0
		float msPerFrame = (1000 * (float)frameTime) / (float)perfCounterFrequency;
		float fps = (float)perfCounterFrequency / (float)frameTime;
		LOG("%.04fms/f,   %.04ff/s\n", msPerFrame, fps);
#endif

		lastFrameTime = endFrameTime;
	}

	return 0;
}