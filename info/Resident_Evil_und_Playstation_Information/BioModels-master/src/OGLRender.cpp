#include "BioModels.h"
#include "Camera.h"
#include "Math.h"
#include "Tim.h"
#include "Render.h"

#include <gl/glcorearb.h>

#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#include <gl/wglext.h>

extern HWND hWND;
extern WNDCLASSEXA wndCLASSEXA;
extern HDC hDC;

#define GETOGLPROC(signature, name) (signature)wglGetProcAddress(name)

void RenderSwapBuffers()
{
	SwapBuffers(hDC);
}

#elif LINUX
#include <GL/gl.h>
#include <GL/glx.h>
#include <gl/glxext.h>
#include <gl/glcorearb.h>

extern Display* display;
extern Window win;

#define GETOGLPROC(signature, name) (signature)glXGetProcAddress((const GLubyte*)name)

void RenderSwapBuffers()
{
	glXSwapBuffers(display, win);
}

#elif EMSCRIPTEN
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#include <emscripten/emscripten.h>
#include <GLFW/glfw3.h>

void RenderSwapBuffers() {}

#endif

static u32 gShader;

#ifndef EMSCRIPTEN

// Shaders
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUNIFORM1IPROC glUniform1i;
// Buffers
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

const char* vertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 mvp;
void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
)";

const char* fragmentShader = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D modelTexture;
void main()
{
	FragColor = texture(modelTexture, TexCoord);
}
)";

#else
const char* vertexShader = R"(
#version 100
precision mediump float;

attribute vec3 aPos;
attribute vec2 aTexCoord;
uniform mat4 mvp;
varying vec2 TexCoord;

void main() 
{
	gl_Position = mvp * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
)";

const char* fragmentShader = R"(
#version 100
precision mediump float;

varying vec2 TexCoord;
uniform sampler2D modelTexture;

void main()
{	
	gl_FragColor = texture2D(modelTexture, TexCoord);
}
)";

#endif

void RenderInit()
{
#ifdef WIN32
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	{
		HWND dummyWND = CreateWindowA(wndCLASSEXA.lpszClassName, 0, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, 0, 0);
		ASSERT(dummyWND);

		HDC dummyDC = GetWindowDC(dummyWND);

		int pixelFormat = ChoosePixelFormat(dummyDC, &pfd);
		bool result = SetPixelFormat(dummyDC, pixelFormat, &pfd);
		ASSERT(result);

		HGLRC dummyglRC = wglCreateContext(dummyDC);
		wglMakeCurrent(dummyDC, dummyglRC);

		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		wglMakeCurrent(0, 0);
		ReleaseDC(dummyWND, dummyDC);
		wglDeleteContext(dummyglRC);
		DestroyWindow(dummyWND);
	}

	hDC = GetWindowDC(hWND);

	const int pixelAttribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0,
	};

	int pixelFormat;
	UINT numFormats;
	wglChoosePixelFormatARB(hDC, pixelAttribList, NULL, 1, &pixelFormat, &numFormats);

	bool result = SetPixelFormat(hDC, pixelFormat, &pfd);
	ASSERT(result);

	const int contextAttribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};

	HGLRC hGLRC = wglCreateContextAttribsARB(hDC, 0, contextAttribList);
	wglMakeCurrent(hDC, hGLRC);
#endif

#ifndef EMSCRIPTEN
	glCreateProgram = GETOGLPROC(PFNGLCREATEPROGRAMPROC, "glCreateProgram");
	glCreateShader = GETOGLPROC(PFNGLCREATESHADERPROC, "glCreateShader");
	glShaderSource = GETOGLPROC(PFNGLSHADERSOURCEPROC, "glShaderSource");
	glCompileShader = GETOGLPROC(PFNGLCOMPILESHADERPROC, "glCompileShader");
	glAttachShader = GETOGLPROC(PFNGLATTACHSHADERPROC, "glAttachShader");
	glLinkProgram = GETOGLPROC(PFNGLLINKPROGRAMPROC, "glLinkProgram");
	glDeleteShader = GETOGLPROC(PFNGLDELETESHADERPROC, "glDeleteShader");
	glGetShaderInfoLog = GETOGLPROC(PFNGLGETSHADERINFOLOGPROC, "glGetShaderInfoLog");
	glGetShaderiv = GETOGLPROC(PFNGLGETSHADERIVPROC, "glGetShaderiv");
	glGetProgramiv = GETOGLPROC(PFNGLGETPROGRAMIVPROC, "glGetProgramiv");
	glUseProgram = GETOGLPROC(PFNGLUSEPROGRAMPROC, "glUseProgram");
	glGetProgramInfoLog = GETOGLPROC(PFNGLGETPROGRAMINFOLOGPROC, "glGetProgramInfoLog");
	glGenBuffers = GETOGLPROC(PFNGLGENBUFFERSPROC, "glGenBuffers");
	glGenVertexArrays = GETOGLPROC(PFNGLGENVERTEXARRAYSPROC, "glGenVertexArrays");
	glBufferData = GETOGLPROC(PFNGLBUFFERDATAPROC, "glBufferData");
	glBindVertexArray = GETOGLPROC(PFNGLBINDVERTEXARRAYPROC, "glBindVertexArray");
	glBindBuffer = GETOGLPROC(PFNGLBINDBUFFERPROC, "glBindBuffer");
	glVertexAttribPointer = GETOGLPROC(PFNGLVERTEXATTRIBPOINTERPROC, "glVertexAttribPointer");
	glEnableVertexAttribArray = GETOGLPROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, "glEnableVertexAttribArray");
	glGetUniformLocation = GETOGLPROC(PFNGLGETUNIFORMLOCATIONPROC, "glGetUniformLocation");
	glUniformMatrix4fv = GETOGLPROC(PFNGLUNIFORMMATRIX4FVPROC, "glUniformMatrix4fv");
	glUniform1i = GETOGLPROC(PFNGLUNIFORM1IPROC, "glUniform1i");
	glDeleteBuffers = GETOGLPROC(PFNGLDELETEBUFFERSPROC, "glDeleteBuffers");
	glDeleteVertexArrays = GETOGLPROC(PFNGLDELETEVERTEXARRAYSPROC, "glDeleteVertexArrays");
#endif

	//#if WIN32
	//	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = GETOGLPROC(PFNWGLSWAPINTERVALEXTPROC, "wglSwapIntervalEXT");
	//	wglSwapIntervalEXT(true);
	//#elif X11
	//	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = GETOGLPROC(PFNGLXSWAPINTERVALEXTPROC, "glXSwapIntervalEXT");
	//	glXSwapIntervalEXT(display, win, true);
	//#endif

	u32 vsShader = glCreateShader(GL_VERTEX_SHADER);
	u32 fsShader = glCreateShader(GL_FRAGMENT_SHADER);

	s32 success;

	glShaderSource(vsShader, 1, &vertexShader, 0);
	glCompileShader(vsShader);
	glGetShaderiv(vsShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char buffer[256];
		glGetShaderInfoLog(vsShader, 256, 0, buffer);
		LOG("Error compiling vertex shader: %s\n", buffer);
		ASSERT(0);
	}

	glShaderSource(fsShader, 1, &fragmentShader, 0);
	glCompileShader(fsShader);
	glGetShaderiv(fsShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		char buffer[256];
		glGetShaderInfoLog(fsShader, 256, 0, buffer);
		LOG("Error compiling fragment shader: %s\n", buffer);
		ASSERT(0);
	}

	gShader = glCreateProgram();
	glAttachShader(gShader, vsShader);
	glAttachShader(gShader, fsShader);
	glLinkProgram(gShader);
	glGetProgramiv(gShader, GL_LINK_STATUS, &success);
	if (!success)
	{
		char buffer[256];
		glGetProgramInfoLog(vsShader, 256, 0, buffer);
		LOG("Error linking program: %s\n", buffer);
		ASSERT(0);
	}

	ASSERT(gShader >= 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderResize(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void RenderBegin()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderEnd()
{
#if _DEBUG
	s16 error = glGetError();
	ASSERT(error == GL_NO_ERROR);
#endif
}

#ifdef EMSCRIPTEN
void RenderSetPolygonMode(u32 mode);

#else
void RenderSetPolygonMode(u32 mode)
{
	switch (mode)
	{
	case RENDER_POLYGON_MODE_FILL:
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
	case RENDER_POLYGON_MODE_LINE:
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	}
	case RENDER_POLYGON_MODE_POINT:
	{
		glPointSize(5.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}
	default:
		ASSERT(0);
	}
}
#endif

void RenderSetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

inline float NormalizeS16(s16 value) {
	return (float)value / 32767.0f;
}

void Texture::Load(void* pixels)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;
}

void Animation::Load(StreamReader* streamReader)
{
	EddHeader clip0;
	streamReader->Read(&clip0, sizeof(EddHeader), 1);

	clipCount = clip0.offset / 4;

	clips = new Clip[clipCount];
	clips[0].beginFrameOffset = 0;
	clips[0].frameCount = clip0.count;

	frameCount = clips[0].frameCount;
	ASSERT(clips[0].beginFrameOffset == 0);
	u32 nextFrameOffset = clips[0].frameCount;
	for (u32 i = 1; i < clipCount; i++)
	{
		streamReader->Read(&clips[i], sizeof(Clip), 1);
		clips[i].beginFrameOffset = nextFrameOffset;
		frameCount += clips[i].frameCount;

		nextFrameOffset = clips[i].frameCount + clips[i].beginFrameOffset;
	}

	frames = new u32[frameCount];
	lastFrameIndex = 0;
	for (u32 i = 0; i < frameCount; i++)
	{
		streamReader->Read(&frames[i], sizeof(u32), 1);
		u16 frameIndex = frames[i] & 0xFFF;
		if (frameIndex > lastFrameIndex)
		{
			lastFrameIndex = frameIndex;
		}
	}
	lastFrameIndex++;
}

void Skeleton::Load(StreamReader* streamReader)
{
	ASSERT(animation.frameCount > 0);
	ASSERT(animation.lastFrameIndex > 0);

	u32 baseOffset = streamReader->GetPos();

	EmrHeader emrHeader;
	streamReader->Read(&emrHeader, sizeof(EmrHeader), 1);

	// Relative position offsets
	boneCount = emrHeader.boneCount;
	Vec3S16* relPos = new Vec3S16[boneCount];
	streamReader->Read(relPos, sizeof(Vec3S16), boneCount);

	// Bones offsets
	u32 bonesOffset = baseOffset + emrHeader.bonesOffset;
	EmrBoneOffset* emrBones = new EmrBoneOffset[boneCount];
	streamReader->SetPos(bonesOffset, SEEK_SET);
	streamReader->Read(emrBones, sizeof(EmrBoneOffset), boneCount);

	bones = new Skeleton::Bone[boneCount];

	for (u32 i = 0; i < boneCount; i++)
	{
		auto* bone = &bones[i];
		bone->relativePosition.x = NormalizeS16(relPos[i].x);
		bone->relativePosition.y = NormalizeS16(relPos[i].y);
		bone->relativePosition.z = NormalizeS16(relPos[i].z);

		u32 partChildrenOffset = bonesOffset + emrBones[i].offset;
		bone->childrenCount = emrBones[i].meshCount;
		bone->children = new u8[bone->childrenCount];

		streamReader->SetPos(partChildrenOffset, SEEK_SET);
		streamReader->Read(bone->children, sizeof(u8), bone->childrenCount);
	}

	// Keyframes
	u32 keyFramesCount = animation.lastFrameIndex;
	streamReader->SetPos(emrHeader.framesOffset + baseOffset, SEEK_SET);
	keyFrames = new KeyFrame[keyFramesCount];
	for (u32 i = 0; i < keyFramesCount; i++)
	{
		Vec3S16 position;
		Vec3S16 speed;
		streamReader->Read(&position, sizeof(Vec3S16), 1);
		streamReader->Read(&speed, sizeof(Vec3S16), 1);

		keyFrames[i].position.x = NormalizeS16(position.x);
		keyFrames[i].position.y = NormalizeS16(position.y);
		keyFrames[i].position.z = NormalizeS16(position.z);
		keyFrames[i].speed.x = NormalizeS16(speed.x);
		keyFrames[i].speed.y = NormalizeS16(speed.y);
		keyFrames[i].speed.z = NormalizeS16(speed.z);

		u32 anglesSize = emrHeader.keyFrameSize - sizeof(Vec3S16) * 2;
		u8* angles = new u8[anglesSize];
		streamReader->Read(angles, anglesSize, 1);

		u32 anglesBitSize = anglesSize * 8;
		u32 angleBitCount = 12;
		u32 angleCount = (anglesBitSize / angleBitCount) / 3;
		BitReader bitReader{ angles };

		for (u32 j = 0; j < angleCount; j++)
		{
			float angleX = (float)bitReader.Read(12);
			float angleY = (float)bitReader.Read(12);
			float angleZ = (float)bitReader.Read(12);

			float angleDegX = DEG12BITSTORAD(angleX);
			float angleDegY = DEG12BITSTORAD(angleY);
			float angleDegZ = DEG12BITSTORAD(angleZ);

			keyFrames[i].angles[j].x = angleDegX;
			keyFrames[i].angles[j].y = angleDegY;
			keyFrames[i].angles[j].z = angleDegZ;
		}

		delete[] angles;
	}

	delete[] relPos;
	delete[] emrBones;
}

void BioModel::Render(
	const Transform& transform,
	float aspectRatio,
	const Camera& camera,
	Skeleton::KeyFrame* keyFrame,
	bool* drawableMeshes
)
{
	ASSERT(drawableMeshes);

	Mat4 model = transform.GetTransform();
	Mat4 view = camera.GetViewMatrix();
	Mat4 projection = camera.GetPerspective(aspectRatio);

	if (keyFrame)
	{
		model = Translate(model, keyFrame->position);
	}

	glUseProgram(gShader);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	Mat4* boneOffsets = new Mat4[meshCount];
	for (u32 i = 0; i < meshCount; i++)
	{
		boneOffsets[i] = Mat4{ 1.0f };
	}

	ComputeMeshOffsets(0, 0, -1, boneOffsets, keyFrame);

	for (u32 i = 0; i < meshCount; i++)
	{
		if (!drawableMeshes[i])
		{
			continue;
		}

		Mat4 meshOffset = model * boneOffsets[i];
		Mat4 mvp = projection * view * meshOffset;

		s16 mvpUniform = glGetUniformLocation(gShader, "mvp");
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &mvp.m00);

		glBindVertexArray(meshes[i].VAO);
		glDrawArrays(GL_TRIANGLES, 0, meshes[i].vertexCount);
		glBindVertexArray(0);
	}

	delete[] boneOffsets;
}

void BioModel::ComputeMeshOffsets(u32 skeletonId, u32 boneId, u32 parentId, Mat4* offsets, Skeleton::KeyFrame* keyFrame)
{
	if (skeletonCount == 0)
	{
		return;
	}

	Mat4& transformMatrix = offsets[boneId];
	if (parentId != -1)
	{
		transformMatrix = offsets[parentId];
	}

	Skeleton::Bone* part = &skeletons[skeletonId].bones[boneId];

	Mat4 translate{ 1.0f };
	translate = Translate(translate, part->relativePosition);
	transformMatrix = transformMatrix * translate;

	if (keyFrame)
	{
		Vec3 angle = keyFrame->angles[boneId];
		transformMatrix = Rotate(transformMatrix, angle.x, Vec3{ 1, 0, 0 });
		transformMatrix = Rotate(transformMatrix, angle.y, Vec3{ 0, 1, 0 });
		transformMatrix = Rotate(transformMatrix, angle.z, Vec3{ 0, 0, 1 });
	}

	offsets[boneId] = transformMatrix;

	for (u32 i = 0; i < part->childrenCount; i++)
	{
		ComputeMeshOffsets(skeletonId, part->children[i], boneId, offsets, keyFrame);
	}
}

static inline void BuildVertex(
	Vertex& vertex,
	const Md1Vertex& coords,
	u8 u,
	u8 v,
	u32 clutId,
	u32 clutWidth,
	const Texture& texture
)
{
	vertex.coord.x = coords.x;
	vertex.coord.y = coords.y;
	vertex.coord.z = coords.z;

	u32 xOffset = clutWidth * (clutId & 3);
	vertex.texture.u = (float)(u + xOffset) / (float)texture.width;
	vertex.texture.v = (float)v / (float)texture.height;
	ASSERT(vertex.texture.u <= 1.0f && vertex.texture.u >= 0.0f);
	ASSERT(vertex.texture.v <= 1.0f && vertex.texture.v >= 0.0f);
}

void BioModel::Load(StreamReader* streamReader)
{
	EmdHeader emd;
	streamReader->Read(&emd, sizeof(EmdHeader), 1);
	streamReader->SetPos(emd.beginOffset, SEEK_SET);

	u32 skeletonOffsets[MAX_SKELETONS] = {};
	u32 animationOffsets[MAX_ANIMATIONS] = {};
	u32 meshOffset = 0;
	u32 textureOffset = 0;

	skeletonCount = 0;

	if (emd.offsetCount == PLD_OFFSET_COUNT)
	{
		ASSERT(emd.offsetCount == PLD_OFFSET_COUNT);

		skeletonCount++;

		streamReader->Read(&animationOffsets[0], sizeof(u32), 1);
		streamReader->Read(&skeletonOffsets[0], sizeof(u32), 1);
		streamReader->Read(&meshOffset, sizeof(u32), 1);
		streamReader->Read(&textureOffset, sizeof(u32), 1);
	}
	else if (emd.offsetCount == EMD_OFFSET_COUNT)
	{
		ASSERT(emd.offsetCount == EMD_OFFSET_COUNT);

		streamReader->SetPos(4, SEEK_CUR);

		for (u32 i = 0; i < MAX_ANIMATIONS; i++)
		{
			streamReader->Read(&animationOffsets[i], sizeof(u32), 1);
			streamReader->Read(&skeletonOffsets[i], sizeof(u32), 1);
		}

		streamReader->Read(&meshOffset, sizeof(u32), 1);
	}
	else
	{
		ASSERT(0);
	}

	// Skeletons and animations
	for (u32 i = 0; i < MAX_ANIMATIONS; i++)
	{
		if (animationOffsets[i] == 0)
		{
			continue;
		}

		Skeleton& skeleton = skeletons[i];
		Animation& animation = skeleton.animation;
		streamReader->SetPos(animationOffsets[i], SEEK_SET);
		animation.Load(streamReader);

		if (animation.frameCount > 0)
		{
			streamReader->SetPos(skeletonOffsets[i], SEEK_SET);
			skeleton.Load(streamReader);
			skeletonCount++;
		}
	}

	// Texture
	// 
	// EMD texture is in TIM file
	if (emd.offsetCount == PLD_OFFSET_COUNT)
	{
		streamReader->SetPos(textureOffset, SEEK_SET);
		void* pixels = LoadTIMAsRGBA(streamReader, texture.width, texture.height, texture.clutCount);
		texture.Load(pixels);
	}

	// Meshes
	Md1Header meshOffsetHeader;
	streamReader->SetPos(meshOffset, SEEK_SET);
	streamReader->Read(&meshOffsetHeader, sizeof(Md1Header), 1);

	// Mesh headers
	u32 baseOffset = streamReader->GetPos();
	meshCount = meshOffsetHeader.meshCount / 2;
	meshes = new BioMesh[meshCount];

	for (u32 i = 0; i < meshCount; i++)
	{
		streamReader->SetPos((u32)baseOffset + i * sizeof(Md1MeshHeader), SEEK_SET);
		Md1MeshHeader meshHeader;
		streamReader->Read(&meshHeader, sizeof(Md1MeshHeader), 1);

		ASSERT(meshHeader.triVertexOffset == meshHeader.quadVertexOffset);
		ASSERT(meshHeader.triVertexCount == meshHeader.quadVertexCount);
		ASSERT(meshHeader.triNormalOffset == meshHeader.quadNormalOffset);
		ASSERT(meshHeader.triNormalCount == meshHeader.quadNormalCount);

		u32 primCount = meshHeader.triCount + meshHeader.quadCount;

		auto* coords = new Md1Vertex[meshHeader.triVertexCount];
		auto* indices = new Md1Indices[primCount];
		auto* textures = new Md1Texture[primCount];

		// Vertexs
		streamReader->SetPos(meshHeader.triVertexOffset + (u32)baseOffset, SEEK_SET);
		streamReader->Read(coords, sizeof(Md1Vertex), meshHeader.triVertexCount);

		// Triangle indices
		streamReader->SetPos(meshHeader.triOffset + (u32)baseOffset, SEEK_SET);
		for (u32 j = 0; j < meshHeader.triCount; j++)
		{
			streamReader->Read(&indices[j], sizeof(Md1Indices) - 2 * sizeof(u16), 1);
		}

		// Quads indices
		streamReader->SetPos(meshHeader.quadOffset + (u32)baseOffset, SEEK_SET);
		for (u32 j = 0; j < meshHeader.quadCount; j++)
		{
			streamReader->Read(&indices[j + meshHeader.triCount], sizeof(Md1Indices), 1);
		}

		// Triangle textures
		streamReader->SetPos(meshHeader.triTextureOffset + (u32)baseOffset, SEEK_SET);
		for (u32 j = 0; j < meshHeader.triCount; j++)
		{
			streamReader->Read(&textures[j], sizeof(Md1Texture) - sizeof(u16) * 2, 1);
		}

		// Quad textures
		streamReader->SetPos(meshHeader.quadTextureOffset + (u32)baseOffset, SEEK_SET);
		for (u32 j = 0; j < meshHeader.quadCount; j++)
		{
			streamReader->Read(&textures[j + meshHeader.triCount], sizeof(Md1Texture), 1);
		}

		// Build vertex buffer
		auto* mesh = &meshes[i];
		mesh->vertexCount = (meshHeader.triCount + (meshHeader.quadCount * 2)) * 3;
		mesh->vertexs = new Vertex[mesh->vertexCount];

		u32 clutWidth = texture.width / texture.clutCount;

		auto* vertex = mesh->vertexs;
		for (u32 j = 0; j < primCount; j++)
		{
			auto* index = &indices[j];
			auto* texCoords = &textures[j];

			bool isQuad = j >= meshHeader.triCount;
			if (!isQuad)
			{
				BuildVertex(*vertex++, coords[index->v0], texCoords->u0, texCoords->v0, texCoords->clutId, clutWidth, texture);
				BuildVertex(*vertex++, coords[index->v1], texCoords->u1, texCoords->v1, texCoords->clutId, clutWidth, texture);
				BuildVertex(*vertex++, coords[index->v2], texCoords->u2, texCoords->v2, texCoords->clutId, clutWidth, texture);
			}
			else
			{
				// First triangle
				BuildVertex(*vertex++, coords[index->v0], texCoords->u0, texCoords->v0, texCoords->clutId, clutWidth, texture);
				BuildVertex(*vertex++, coords[index->v1], texCoords->u1, texCoords->v1, texCoords->clutId, clutWidth, texture);
				BuildVertex(*vertex++, coords[index->v3], texCoords->u3, texCoords->v3, texCoords->clutId, clutWidth, texture);

				// Second triangle
				BuildVertex(*vertex++, coords[index->v0], texCoords->u0, texCoords->v0, texCoords->clutId, clutWidth, texture);
				BuildVertex(*vertex++, coords[index->v2], texCoords->u2, texCoords->v2, texCoords->clutId, clutWidth, texture);
				BuildVertex(*vertex++, coords[index->v3], texCoords->u3, texCoords->v3, texCoords->clutId, clutWidth, texture);
			}
		}

		glGenVertexArrays(1, &mesh->VAO);
		glGenBuffers(1, &mesh->VBO);

		glBindVertexArray(mesh->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(Vertex), mesh->vertexs, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, coord));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

		delete[] coords;
		delete[] indices;
		delete[] textures;
		delete[] mesh->vertexs;
	}

	isLoaded = true;
}