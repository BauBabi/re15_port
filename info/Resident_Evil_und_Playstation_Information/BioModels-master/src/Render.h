#include "BioModels.h"

#include "Math.h"
#include "Utils.h"

#ifndef RENDER_H
#define RENDER_H

#define MAX_SKELETONS 3

struct Texture
{
	u32 width;
	u32 height;
	u32 clutCount;

	// TODO: Make render opengl agnostic
	u32 id;

	void Load(void* pixels);
};

struct Vertex
{
	// TODO: normals
	Vec3S16 coord;
	Vec2 texture;
};

struct BioMesh
{
	Vertex* vertexs;
	u32 vertexCount;

	// TODO: Make render opengl agnostic
	u32 VAO, VBO;
};

struct Animation
{
	struct Clip
	{
		u16 frameCount;
		u16 beginFrameOffset;
	};

	u32 frameCount;
	u32* frames;
	u16 lastFrameIndex;

	u32 clipCount;
	Clip* clips;

	void Load(StreamReader* streamReader);
};

struct Skeleton
{
	struct Bone
	{
		u8* children;
		u32 childrenCount;

		Vec3 relativePosition;
	};

	struct KeyFrame
	{
		Vec3 position;
		Vec3 speed;
		// TODO: Figure out max angles
		Vec3 angles[200];
	};

	Bone* bones;
	u32 boneCount;

	Animation animation;
	KeyFrame* keyFrames;

	void Load(StreamReader* streamReader);
};

struct Transform
{
	Vec3 translation;
	Vec3 rotation;
	Vec3 scale;

	inline Mat4 GetTransform() const
	{
		Mat4 identity{ 1.0f };
		Mat4 rotate =
			Rotate(identity, DEGTORAD(rotation.z), { 0, 0, 1 })
			* Rotate(identity, DEGTORAD(rotation.y), { 0, 1, 0 })
			* Rotate(identity, DEGTORAD(rotation.x), { 1, 0, 0 });


		Mat4 transform =
			Translate(identity, translation)
			* rotate;

		transform = Scale(transform, scale);

		return transform;
	}
};

struct BioModel
{
	u32 meshCount;
	BioMesh* meshes;
	Skeleton skeletons[MAX_SKELETONS];
	u32 skeletonCount;
	Texture texture;
	bool isLoaded;

	void Render(
		const Transform& transform,
		float aspectRatio,
		const Camera& camera,
		Skeleton::KeyFrame* keyFrame,
		bool* drawableMeshes
	);

	void Load(StreamReader* streamReader);
	void ComputeMeshOffsets(u32 skeletonId, u32 boneId, u32 parentId, Mat4* offsets, Skeleton::KeyFrame* keyFrame);
};

void RenderInit();
void RenderResize(int x, int y, int width, int height);
void RenderSwapBuffers();
void RenderBegin();
void RenderEnd();
void RenderSetClearColor(float r, float g, float b, float a);

enum
{
	RENDER_POLYGON_MODE_FILL,
	RENDER_POLYGON_MODE_LINE,
	RENDER_POLYGON_MODE_POINT,
};
void RenderSetPolygonMode(u32 mode);

#endif