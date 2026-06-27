#include "BioModels.h"

#include "Camera.h"
#include "Utils.h"
#include "Render.h"
#include "Tim.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <IconsFontAwesome5.h>

#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif LINUX
#include <dirent.h>

#elif EMSCRIPTEN
#include <emscripten.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

extern GLFWwindow* window;
#endif

struct State
{
	bool initialized;
	Camera initialCamera;
	Camera camera;
	u32 activeModel;
	bool* drawableMeshes;
	bool renderStaticMeshes;
	u32 activeSkeleton;
	Animation::Clip* activeClip;
	int activeClipIndex;
	int animationSpeed;
	float animationTime;
	u32 animationFrame;
	bool pauseAnimation;
	bool reverseAnimation;
	int gizmoOperation;

	Transform transform;
	Transform defaultTransform;

	BioModel models[MODEL_COUNT];
};

static State gState;

static inline ModelId GetModelIdFromStr(const char* idStr)
{
	for (u32 i = 0; i < ARRAY_SIZE(modelFileTable); i++)
	{
		if (StrIgnoreCaseEquals(modelFileTable[i].filename, idStr))
		{
			return modelFileTable[i].id;
		}
	}

	return UNKNOWN;
}

static inline u32 GetStaticMeshCount()
{
	BioModel& model = gState.models[gState.activeModel];

	u32 boneCount = model.skeletons[gState.activeSkeleton].boneCount;
	u32 staticMeshCount = model.meshCount - boneCount;

	return staticMeshCount;
}

static inline void SetActiveModel(u32 index)
{
	ASSERT(index >= 0 && index < MODEL_COUNT);
	BioModel& model = gState.models[index];

	gState.activeClip = nullptr;
	gState.activeModel = index;
	gState.activeSkeleton = 0;
	delete[] gState.drawableMeshes;
	gState.drawableMeshes = new bool[model.meshCount];

	// Show skinned meshes and hide statics ones
	u32 skeletonMeshCount = model.skeletons->boneCount;
	memset(gState.drawableMeshes, true, sizeof(bool) * skeletonMeshCount);
	memset(&gState.drawableMeshes[skeletonMeshCount], false, sizeof(bool) * GetStaticMeshCount());
	gState.renderStaticMeshes = false;
	gState.transform = gState.defaultTransform;
}

static void LoadModel(const char* filepath, const char* extension, ModelId modelId)
{
	ASSERT(modelId != ModelId::UNKNOWN);

	BioModel& model = gState.models[modelId];
	if (model.isLoaded)
	{
		return;
	}

	// EMD texture is in TIM file
	if (StrIgnoreCaseEquals(extension, FileExtension::EMD))
	{
		char timPath[256];
		strcpy(timPath, filepath);

		auto len = strlen(filepath);
		timPath[len - 3] = 'T';
		timPath[len - 2] = 'I';
		timPath[len - 1] = 'M';

		Texture& texture = model.texture;

		FileStreamReader timStreamReader;
#ifndef LINUX
		timStreamReader.Load(timPath);
#else
		// (Linux) Case-insensitive handling
		// Some models use uppercase in the TIM file name and lowercase in the EMD file name.
		// Example:
		// em040.emd
		// EM040.TIM
		//
		// This needs to be handled explicitly on Linux, as file comparisons are case-sensitive,
		// unlike on Windows.
		char directoryPath[NAME_MAX];
		const char* slashIndex = strrchr(filepath, '/');
		size_t directoryLen = slashIndex - filepath;
		strncpy(directoryPath, filepath, directoryLen);
		directoryPath[directoryLen] = '\0';

		DIR* directory = opendir(directoryPath);
		ASSERT(directory);

		// Find the matching TIM file
		dirent* entry;
		while ((entry = readdir(directory)) != NULL)
		{
			char fullPath[PATH_MAX];
			sprintf(fullPath, "%s/%s", directoryPath, entry->d_name);

			if (StrIgnoreCaseEquals(timPath, fullPath))
			{
				timStreamReader.Load(fullPath);
				break;
			}
		}
		ASSERT(timStreamReader.file);
		closedir(directory);
#endif
		void* pixels = LoadTIMAsRGBA(&timStreamReader, texture.width, texture.height, texture.clutCount);
		texture.Load(pixels);
	}

	FileStreamReader fileStreamReader{ filepath };
	model.Load(&fileStreamReader);
}

static bool LoadModels(const char* directoryPath)
{
#if WIN32
	DWORD attrs = GetFileAttributesA(directoryPath);
	if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY))
	{
		return false;
	}

	WIN32_FIND_DATAA findResult;

	char searchExpression[MAX_PATH];
	sprintf(searchExpression, "%s\\*", directoryPath);

	HANDLE findHandle = FindFirstFileA(searchExpression, &findResult);
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		ASSERT(0);
	}

	do
	{
		if (strcmp(findResult.cFileName, ".") == 0 || strcmp(findResult.cFileName, "..") == 0)
		{
			continue;
		}
		char fullPath[MAX_PATH];
		sprintf(fullPath, "%s\\%s", directoryPath, findResult.cFileName);

		if (findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			LoadModels(fullPath);
		}
		else
		{
			const char* extension = strrchr(findResult.cFileName, '.');
			// Skip if file is not .PLD or .EMD
			if (!StrIgnoreCaseEquals(extension, FileExtension::PLD) && !StrIgnoreCaseEquals(extension, FileExtension::EMD))
			{
				continue;
			}

			char modelIdStr[100];
			sprintf(modelIdStr, "%s", findResult.cFileName);
			if (StrIgnoreCaseEquals(extension, FileExtension::EMD))
			{
				modelIdStr[2] = '0';
			}

			ModelId modelId = GetModelIdFromStr(modelIdStr);
			if (modelId == ModelId::UNKNOWN)
			{
				LOG("Unknown model: %s\n", modelIdStr);
				continue;
			}

			LOG("Loading model(%d): %s\n", modelId, modelIdStr);
			LoadModel(fullPath, extension, modelId);
		}
	} while (FindNextFileA(findHandle, &findResult));

	FindClose(findHandle);
	return true;

#elif LINUX
	DIR* directory = opendir(directoryPath);
	if (!directory)
	{
		return false;
	}

	dirent* entry;
	while ((entry = readdir(directory)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}

		char fullPath[PATH_MAX];
		sprintf(fullPath, "%s/%s", directoryPath, entry->d_name);

		if (entry->d_type == DT_DIR)
		{
			LoadModels(fullPath);
		}
		if (entry->d_type == DT_REG)
		{
			const char* extension = strrchr(entry->d_name, '.');
			// Skip if file is not .PLD or .EMD
			if (!StrIgnoreCaseEquals(extension, FileExtension::PLD) && !StrIgnoreCaseEquals(extension, FileExtension::EMD))
			{
				continue;
			}

			char modelIdStr[PATH_MAX];
			sprintf(modelIdStr, "%s", entry->d_name);
			if (StrIgnoreCaseEquals(extension, FileExtension::EMD))
			{
				modelIdStr[2] = '0';
			}

			ModelId modelId = GetModelIdFromStr(modelIdStr);
			if (modelId == ModelId::UNKNOWN)
			{
				LOG("Unknown model: %s\n", entry->d_name);
				continue;
			}

			LOG("Loading model(%d): %s\n", modelId, modelIdStr);
			LoadModel(fullPath, extension, modelId);
		}
	}

	closedir(directory);

#elif EMSCRIPTEN
	LoadModel("em050.emd", FileExtension::EMD, LEON_DEFAULT);
#endif

	return true;
}

#ifdef EMSCRIPTEN

EM_JS(void, JSAlert, (const char* message),
	{
		alert(UTF8ToString(message));
	}
);

extern "C" {
	void JSLoadModel(const char* filepath, u8* stream, u8* timStream)
	{
		ASSERT(stream);

		ModelId modelId = GetModelIdFromStr(filepath);
		if (modelId == ModelId::UNKNOWN)
		{
			char buffer[100];
			sprintf(buffer, "Model '%s' is not supported, try uploading other file\n", filepath);
			JSAlert(buffer);
			return;
		}

		BioModel& model = gState.models[modelId];
		if (model.isLoaded)
		{
			return;
		}

		if (timStream)
		{
			LOG("WebGL loading TIM...\n");
			MemoryStreamReader timStreamReader{ timStream };
			Texture& texture = model.texture;

			void* pixels = LoadTIMAsRGBA(&timStreamReader, texture.width, texture.height, texture.clutCount);
			texture.Load(pixels);
		}

		LOG("WebGL loading EMD...\n");
		MemoryStreamReader streamReader{ stream };
		model.Load(&streamReader);

		SetActiveModel(modelId);
	}
}

#endif

static inline void SetActiveClip(u32 clipIndex, u32 skeletonIndex)
{
	BioModel* model = &gState.models[gState.activeModel];

	gState.activeClipIndex = clipIndex;
	gState.activeSkeleton = skeletonIndex;

	Skeleton* skeleton = &model->skeletons[gState.activeSkeleton];
	gState.activeClip = &skeleton->animation.clips[clipIndex];
	gState.animationFrame = gState.activeClip->beginFrameOffset;
	gState.reverseAnimation = false;
}

static inline void SetBindPose()
{
	gState.activeClip = nullptr;
	gState.animationFrame = 0;
}

static inline Skeleton::KeyFrame* GetKeyFrame()
{
	ASSERT(gState.activeClip);
	BioModel* model = &gState.models[gState.activeModel];
	Skeleton* skeleton = &model->skeletons[gState.activeSkeleton];

	u32 frame = skeleton->animation.frames[gState.animationFrame];
	u16 keyFrameIndex = frame & 0xFFF;

	return &skeleton->keyFrames[keyFrameIndex];
}

static inline u16 GetActiveClipLastFrame()
{
	ASSERT(gState.activeClip);
	return (gState.activeClip->beginFrameOffset + gState.activeClip->frameCount);
}

static inline void ClipNextFrame()
{
	ASSERT(gState.activeClip);

	u16 lastFrame = GetActiveClipLastFrame();
	if (++gState.animationFrame == lastFrame)
	{
		gState.animationFrame = gState.activeClip->beginFrameOffset;
	}
}

static inline void ClipPreviousFrame()
{
	ASSERT(gState.activeClip);

	u16 beginFrame = gState.activeClip->beginFrameOffset;
	if (--gState.animationFrame <= beginFrame)
	{
		gState.animationFrame = GetActiveClipLastFrame() - 1;
	}
}

static inline bool ImGuiButton(const char* label, bool disabled = false)
{
	constexpr ImVec2 size{ 30, 30 };

	if (disabled)
	{
		ImGui::BeginDisabled();
		bool pressed = ImGui::Button(label, size);
		ImGui::EndDisabled();
		return pressed;
	}

	return ImGui::Button(label, size);
}

static inline bool ImGuiInput(const char* label, int* data, bool disabled = false)
{
	if (disabled)
	{
		ImGui::BeginDisabled();
		bool pressed = ImGui::InputInt(label, data);
		ImGui::EndDisabled();
		return pressed;
	}

	return ImGui::InputInt(label, data);
}

static inline void ImGuiInputVec3(const char* label, Vec3& vec3, const char* id)
{
	char tableIdBuffer[100];
	sprintf(tableIdBuffer, "%s%s", id, "_table");

	if (ImGui::BeginTable("##aaa", 4))
	{
		char xIdBuffer[100];
		char yIdBuffer[100];
		char zIdBuffer[100];
		sprintf(xIdBuffer, "%s%s", id, "_x");
		sprintf(yIdBuffer, "%s%s", id, "_y");
		sprintf(zIdBuffer, "%s%s", id, "_z");

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label);

		// X
		ImGui::TableSetColumnIndex(1);

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::TextUnformatted("X");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputFloat(xIdBuffer, &vec3.x);

		// Y
		ImGui::TableSetColumnIndex(2);

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::TextUnformatted("Y");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputFloat(yIdBuffer, &vec3.y);

		// Z
		ImGui::TableSetColumnIndex(3);

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 150, 255, 255));
		ImGui::TextUnformatted("Z");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputFloat(zIdBuffer, &vec3.z);

		ImGui::EndTable();
	}
}

static void RenderUI(u32 width, u32 height)
{
	ASSERT(gState.activeModel <= MODEL_COUNT - 1);
	BioModel model = gState.models[gState.activeModel];

	constexpr float windowWidth = 400.0f;

	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	{
		ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2{ windowWidth, (float)height }, ImGuiCond_Always);

		ImGui::Begin("BioModels", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

		// Model selector
		{
			ImGui::SeparatorText("Model");
			const char* comboPreviewValue = modelNameTable[gState.activeModel];
			if (ImGui::BeginCombo("##model_combo", comboPreviewValue, ImGuiComboFlags_HeightLargest))
			{
				for (int i = 0; i < MODEL_COUNT; i++)
				{
					if (!gState.models[i].isLoaded)
					{
						continue;
					}

					bool isSelected = (gState.activeModel == i);
					if (ImGui::Selectable(modelNameTable[i], isSelected))
					{
						SetActiveModel(i);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::SameLine();
			char buffer[256];
			sprintf(buffer, "(%s)", modelFileTable[gState.activeModel].filename);
			ImGui::Text("%s", buffer);
		}

		// Transform
		{
			const char* transformId = "Transform";

			// Close transform section
			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				ImGui::GetStateStorage()->SetInt(ImGui::GetID(transformId), 0);

			}
			if (ImGui::CollapsingHeader(transformId, ImGuiTreeNodeFlags_None))
			{
				SetBindPose();

				ImGui::Spacing();
				if (ImGui::Button("Reset transform"))
				{
					gState.transform = gState.defaultTransform;
				}

				char buffer[50];
				sprintf(buffer, "%s %s", "Help", ICON_FA_QUESTION_CIRCLE);
				ImGui::SeparatorText(buffer);

				if (ImGui::BeginItemTooltip())
				{
					ImGui::BulletText("Press 'S' to scale");
					ImGui::BulletText("Press 'T' to translate");
					ImGui::BulletText("Press 'R' to rotate");
					ImGui::BulletText("Press 'Escape' to abort transformation");
					ImGui::Spacing();
					ImGui::Text("%s %s", ICON_FA_EXCLAMATION_TRIANGLE, "Camera is locked in transformation mode");
					ImGui::Text("%s %s", ICON_FA_EXCLAMATION_TRIANGLE, "Close transformation for playing animations");
					ImGui::Spacing();
					ImGui::Separator();

					ImGui::EndTooltip();
				}

				ImGuiInputVec3("Translation", gState.transform.translation, "##translation");
				ImGuiInputVec3("Rotation", gState.transform.rotation, "##rotation");
				ImGuiInputVec3("Scale", gState.transform.scale, "##scale");

				auto windowPos = ImGui::GetWindowPos();
				ImGuizmo::SetRect(windowPos.x, windowPos.y, (float)width, (float)height);

				Mat4 view = gState.camera.GetViewMatrix();
				Mat4 projection = gState.camera.GetPerspective((float)width / (float)height);

				if (gState.gizmoOperation == -1)
				{
					gState.gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}
				if (ImGui::IsKeyPressed(ImGuiKey_T) && !ImGuizmo::IsUsing())
				{
					gState.gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}
				if (ImGui::IsKeyPressed(ImGuiKey_S) && !ImGuizmo::IsUsing())
				{
					gState.gizmoOperation = ImGuizmo::OPERATION::SCALE;
				}
				if (ImGui::IsKeyPressed(ImGuiKey_R) && !ImGuizmo::IsUsing())
				{
					gState.gizmoOperation = ImGuizmo::OPERATION::ROTATE_Y;
				}

				ImGuizmo::SetOrthographic(false);

				Mat4 transform = gState.transform.GetTransform();
				ImGuizmo::Manipulate(&view.m00, &projection.m00, (ImGuizmo::OPERATION)gState.gizmoOperation, ImGuizmo::WORLD, &transform.m00);

				if (ImGuizmo::IsUsing())
				{
					Vec3 translation;
					Vec3 rotation;
					Vec3 scale;

					ImGuizmo::DecomposeMatrixToComponents(
						&transform.m00,
						&translation.x,
						&rotation.x,
						&scale.x
					);

					if (gState.gizmoOperation == ImGuizmo::OPERATION::SCALE)
					{
						gState.transform.scale = scale;
					}
					else if (gState.gizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
					{
						gState.transform.translation = translation;
					}
					else if (gState.gizmoOperation == ImGuizmo::OPERATION::ROTATE_Y)
					{
						gState.transform.rotation = rotation;
					}
				}
			}
			else
			{
				gState.gizmoOperation = -1;
			}
		}

		// Mesh selector
		{
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Mesh selector", ImGuiTreeNodeFlags_None))
			{
				ImGui::SeparatorText("Skinned Meshes");

				// Skinned meshes
				u32 skeletonMeshCount = model.skeletons[gState.activeSkeleton].boneCount;
				if (ImGui::Button("All"))
				{
					memset(gState.drawableMeshes, true, sizeof(bool) * skeletonMeshCount);
				}
				ImGui::SameLine();
				if (ImGui::Button("None"))
				{
					memset(gState.drawableMeshes, false, sizeof(bool) * skeletonMeshCount);
				}

				for (u32 i = 0; i < skeletonMeshCount; i++)
				{
					char meshName[100];
					sprintf(meshName, "Mesh %d", i);

					bool checked = gState.drawableMeshes[i] != 0;
					if (ImGui::Checkbox(meshName, &checked))
					{
						gState.drawableMeshes[i] = checked ? 1 : 0;
					}
					// 3 meshes per line
					if ((i + 1) % 3 != 0)
					{
						ImGui::SameLine();
					}
				}

				// Static meshes
				u32 staticMeshCount = GetStaticMeshCount();
				if (staticMeshCount)
				{
					char buffer[50];
					sprintf(buffer, "%s %s", "Static Meshes", ICON_FA_QUESTION_CIRCLE);
					ImGui::SeparatorText(buffer);
					ImGui::SetItemTooltip("Render non-skinned meshes (not part of the skeleton hierarchy)");
					if (ImGui::Button("All##x"))
					{
						memset(&gState.drawableMeshes[skeletonMeshCount], true, sizeof(bool) * staticMeshCount);
					}
					ImGui::SameLine();
					if (ImGui::Button("None##x"))
					{
						memset(&gState.drawableMeshes[skeletonMeshCount], false, sizeof(bool) * staticMeshCount);
					}

					for (u32 i = skeletonMeshCount; i < skeletonMeshCount + staticMeshCount; i++)
					{
						char meshName[100];
						sprintf(meshName, "Mesh %d", i);

						bool checked = gState.drawableMeshes[i] != 0;
						if (ImGui::Checkbox(meshName, &checked))
						{
							gState.drawableMeshes[i] = checked ? 1 : 0;
						}
						// 3 meshes per line
						if ((i + 1) % 3 != 0)
						{
							ImGui::SameLine();
						}
					}
				}
			}
		}
#ifndef EMSCRIPTEN
		// Polygon mode
		{
			ImGui::Spacing();
			ImGui::SeparatorText("Polygon mode");
			static int polygonMode = RENDER_POLYGON_MODE_FILL;
			if (ImGui::RadioButton("Fill", &polygonMode, RENDER_POLYGON_MODE_FILL))
			{
				RenderSetPolygonMode(RENDER_POLYGON_MODE_FILL);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Line", &polygonMode, RENDER_POLYGON_MODE_LINE))
			{
				RenderSetPolygonMode(RENDER_POLYGON_MODE_LINE);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Point", &polygonMode, RENDER_POLYGON_MODE_POINT))
			{
				RenderSetPolygonMode(RENDER_POLYGON_MODE_POINT);
			}
			ImGui::Separator();
		}
#endif

		// Camera
		{
			ImGui::Spacing();
			if (ImGui::Button("Reset camera"))
			{
				gState.camera = gState.initialCamera;
			}
		}

		// Texture
		{
			ImGui::SeparatorText("Texture");
			ImGui::Spacing();
			ImGui::Image((ImTextureID)(intptr_t)model.texture.id, ImVec2(windowWidth, (float)model.texture.height));
		}

		// Animations
		{
			if (model.skeletonCount > 0)
			{
				// Disable when no frames or not selected animation
				bool disableAnimControls = !gState.activeClip || gState.activeClip->frameCount == 1;

				ImGui::SeparatorText("Animation");
				ImGui::SetNextItemWidth(100);
				ImGuiInput("##anim_speed", &gState.animationSpeed, disableAnimControls);
				ImGui::SameLine();
				ImGui::Text("Frames per second");

				if (ImGuiButton(ICON_FA_BACKWARD, disableAnimControls))
				{
					ClipPreviousFrame();
				}
				ImGui::SameLine();
				if (gState.pauseAnimation)
				{
					if (ImGuiButton(ICON_FA_PLAY, (gState.activeClip && !gState.pauseAnimation) || disableAnimControls))
					{
						gState.pauseAnimation = false;
					}
				}
				else
				{
					if (ImGuiButton(ICON_FA_PAUSE, (gState.activeClip && gState.pauseAnimation) || disableAnimControls))
					{
						gState.pauseAnimation = true;
					}
				}
				ImGui::SameLine();
				if (ImGuiButton(ICON_FA_FORWARD, disableAnimControls))
				{
					ClipNextFrame();
				}
				ImGui::SameLine();
				if (ImGuiButton(gState.reverseAnimation ? ICON_FA_REDO : ICON_FA_UNDO, disableAnimControls))
				{
					gState.reverseAnimation = !gState.reverseAnimation;
				}
				ImGui::SetItemTooltip(gState.reverseAnimation ? "Forward" : "Reverse");

				if (!disableAnimControls)
				{
					ImGui::SameLine();

					char buffer[50];
					u16 currentFrameIndex = (gState.animationFrame - gState.activeClip->beginFrameOffset);
					sprintf(buffer, "Frame %d of %d", currentFrameIndex, gState.activeClip->frameCount - 1);
					ImGui::Text("%s", buffer);
				}

				static int selected = 0;

				char comboPreviewValue[30];
				if (gState.activeClip)
				{
					u32 clipOffset = 0;
					if (gState.activeSkeleton > 0)
					{
						for (u32 i = 0; i < gState.activeSkeleton; i++)
						{
							clipOffset += model.skeletons[i].animation.clipCount;
						}
					}
					bool noFrames = gState.activeClip->frameCount == 1;
					u32 animIndex = gState.activeClipIndex + clipOffset;
					sprintf(comboPreviewValue, "Animation %d%s", animIndex, noFrames ? " (no frames)" : "");
				}
				else
				{
					sprintf(comboPreviewValue, "Select animation to play");
				}

				if (ImGui::BeginCombo("##listbox_animation", comboPreviewValue, ImGuiComboFlags_HeightLargest))
				{
					u32 animationIndex = 0;

					for (u32 skeleton = 0; skeleton < model.skeletonCount; skeleton++)
					{
						for (u32 clip = 0; clip < model.skeletons[skeleton].animation.clipCount; clip++)
						{
							bool isSelected = (selected == animationIndex);
							ImGuiSelectableFlags flags = (selected == animationIndex) ? ImGuiSelectableFlags_Highlight : 0;

							Animation::Clip& animClip = model.skeletons[skeleton].animation.clips[clip];
							bool noFrames = animClip.frameCount == 1;
							char animationName[128];
							sprintf(animationName, "Animation %d%s", animationIndex, noFrames ? " (no frames)" : "");

							if (ImGui::Selectable(animationName, isSelected, flags))
							{
								selected = animationIndex;
								SetActiveClip(clip, skeleton);
							}
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}

							animationIndex++;
						}
					}
					ImGui::EndCombo();
				}
			}

			// Background color and github project link
			{
				static float color[3] = { 0.0f, 0.0f, 0.0f };
				ImGui::SeparatorText("Background color: ");
				if (ImGui::ColorEdit3("Color", color))
				{
					RenderSetClearColor(color[0], color[1], color[2], 1.0f);
				}
				ImGui::Spacing();
				ImGui::TextLinkOpenURL("About", "https://github.com/elmarsan/BioModels");
			}
		}

#if 0
		static bool demo = true;
		ImGui::ShowDemoWindow(&demo);
#endif
		ImGui::End();
	}

	ImGui::EndFrame();
}

bool Update(Input& input, float delta)
{
	if (!gState.initialized)
	{
		gState.initialized = true;
		gState.activeModel = 0;
		if (!LoadModels("data"))
		{
			BIO_ERROR("data folder not found\n");
			return false;
		}

		gState.camera.position = Vec3{ 0, 0, -0.2f };
		gState.camera.front = Vec3{ 0, 0, -1.0f };
		gState.camera.up = Vec3{ 0, 1.0f, 0 };
		gState.initialCamera = gState.camera;

		gState.activeModel = LEON_DEFAULT;

		gState.animationTime = 0.0f;
		gState.animationSpeed = 30;

		BioModel model = gState.models[gState.activeModel];
		gState.drawableMeshes = new bool[model.meshCount];
		memset(gState.drawableMeshes, true, sizeof(bool) * model.meshCount);
		gState.renderStaticMeshes = false;

		constexpr float scaleFactor = 8.0f;
		gState.transform.translation = Vec3{ 0.1f, -0.5f, -2.0f };
		gState.transform.rotation = Vec3{ 0, -90.0f, 180.0f };
		gState.transform.scale = Vec3{ scaleFactor, scaleFactor, scaleFactor };
		gState.defaultTransform = gState.transform;

		gState.gizmoOperation = -1;
	}

	// Lock the camera when applying transformations
	if (gState.gizmoOperation == -1)
	{
		constexpr float cameraVelocity = 2.2f;

		if (input.inputKeys[INPUT_KEY_CODE_W].isDown)
		{
			gState.camera.MoveForward(delta, cameraVelocity);
		}
		if (input.inputKeys[INPUT_KEY_CODE_S].isDown)
		{
			gState.camera.MoveBackward(delta, cameraVelocity);
		}
		if (input.inputKeys[INPUT_KEY_CODE_A].isDown)
		{
			gState.camera.MoveLeft(delta, cameraVelocity);
		}
		if (input.inputKeys[INPUT_KEY_CODE_D].isDown)
		{
			gState.camera.MoveRight(delta, cameraVelocity);
		}
	}

	constexpr float yRotationDeg = 150.0f;
	if (input.inputKeys[INPUT_KEY_CODE_Q].isDown)
	{
		gState.transform.rotation.y -= delta * yRotationDeg;
	}
	if (input.inputKeys[INPUT_KEY_CODE_E].isDown)
	{
		gState.transform.rotation.y += delta * yRotationDeg;
	}

	if (gState.activeClip && !gState.pauseAnimation)
	{
		gState.animationTime += delta;

		if (gState.animationTime >= 1.0f / (float)gState.animationSpeed)
		{
			gState.animationTime = 0;

			if (gState.reverseAnimation)
			{
				ClipPreviousFrame();
			}
			else
			{
				ClipNextFrame();
			}
		}
	}

	return true;
}

void Render(float delta, u32 width, u32 height)
{
	BioModel currentModel = gState.models[gState.activeModel];

	RenderBegin();
	RenderUI(width, height);
	float aspect = (float)width / (float)height;

	Skeleton::KeyFrame* keyFrame = nullptr;
	if (gState.activeClip)
	{
		keyFrame = GetKeyFrame();
	}

	currentModel.Render(gState.transform, aspect, gState.camera, keyFrame, gState.drawableMeshes);
	RenderEnd();
}

void SetupImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

#ifdef EMSCRIPTEN
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
#endif

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#ifndef _DEBUG
	io.ConfigDebugHighlightIdConflicts = false;
#endif

	float baseFontSize = 19.5f;
	io.Fonts->AddFontFromFileTTF("DroidSans.ttf", baseFontSize);

	float iconFontSize = baseFontSize * 2.0f / 3.0f;
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, iconFontSize, &icons_config, icons_ranges);

	ImGui::StyleColorsClassic();
}