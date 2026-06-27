#include "..\..\BioModels.h"
#include "..\..\OGLRender.cpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <IconsFontAwesome5.h>
#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

GLFWwindow* window;

static Input gInput;
float gDelta = 0.0f;
float gLastFrameTime = 0.0f;

EM_JS(void, JSGetCanvasDimensions, (int* width, int* height),
	{
		setValue(width, Module.canvas.width, 'i32');
		setValue(height, Module.canvas.height, 'i32');
	}
);

void MainLoop()
{
	float currentTime = (float)emscripten_get_now();
	gDelta = (currentTime - gLastFrameTime) / 1000.0f;
	gLastFrameTime = currentTime;

	int width;
	int height;
	JSGetCanvasDimensions(&width, &height);

	if (!Update(gInput, gDelta))
	{
		emscripten_cancel_main_loop();
	}
	else
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		Render(gDelta, width, height);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

#if 0
	float msPerFrame = gDelta * 1000.0f;
	float fps = 1.0f / gDelta;
	LOG("%.04fms/f,   %.04ff/s\n", msPerFrame, fps);
#endif
}

inline void UpdateInputKey(Input& input, u32 keyCode, bool isDown, bool wasDown)
{
	ASSERT(keyCode >= INPUT_KEY_CODE_Q && keyCode < INPUT_KEY_COUNT);

	gInput.inputKeys[keyCode].isDown = isDown;
	gInput.inputKeys[keyCode].wasDown = wasDown;
}

bool KeyCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
{
	u32 keyCode = -1;

	if (e->keyCode == DOM_VK_W)
	{
		keyCode = INPUT_KEY_CODE_W;
	}
	if (e->keyCode == DOM_VK_S)
	{
		keyCode = INPUT_KEY_CODE_S;
	}
	if (e->keyCode == DOM_VK_A)
	{
		keyCode = INPUT_KEY_CODE_A;
	}
	if (e->keyCode == DOM_VK_D)
	{
		keyCode = INPUT_KEY_CODE_D;
	}
	if (e->keyCode == DOM_VK_Q)
	{
		keyCode = INPUT_KEY_CODE_Q;
	}
	if (e->keyCode == DOM_VK_E)
	{
		keyCode = INPUT_KEY_CODE_E;
	}

	if (keyCode == -1)
	{
		return false;
	}

	bool isDown = eventType == EMSCRIPTEN_EVENT_KEYDOWN;
	bool wasDown = isDown && gInput.inputKeys[keyCode].isDown;

	UpdateInputKey(gInput, keyCode, isDown, wasDown);
	return true;
}

int main()
{
	ASSERT(glfwInit());
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1366, 768, "BioModels", 0, 0);
	ASSERT(window);

	glfwMakeContextCurrent(window);

	RenderInit();
	SetupImGui();

	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, 1, KeyCallback);
	emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, 1, KeyCallback);
	emscripten_set_main_loop(MainLoop, 0, 1);
}