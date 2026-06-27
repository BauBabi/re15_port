#define _CRT_SECURE_NO_WARNINGS

#ifndef BIOMODELS_H
#define BIOMODELS_H

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#if _DEBUG
#include <assert.h>
#define ASSERT(expression) assert(expression)
#define LOG(...) printf(__VA_ARGS__)
#else
#define ASSERT(expression)
#define LOG(...)
#endif

#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define BIO_ERROR(msg) MessageBoxA(0, msg, "BioModels", MB_OK | MB_ICONERROR)
#else
#define BIO_ERROR(msg) printf(msg)
#endif

#define PI 3.14159265359f
#define DEGTORAD(deg) ((deg) * (PI) / 180.0f)
#define DEG12BITSTORAD(deg) ((deg / 4096) * (PI * 2))
#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

enum
{
	INPUT_KEY_CODE_Q,
	INPUT_KEY_CODE_E,
	INPUT_KEY_CODE_W,
	INPUT_KEY_CODE_S,
	INPUT_KEY_CODE_A,
	INPUT_KEY_CODE_D,

	INPUT_KEY_COUNT
};

struct InputKey
{
	bool isDown = false;
	bool wasDown = false;
};

struct Input
{
	InputKey inputKeys[INPUT_KEY_COUNT];
};

enum ModelId : u8
{
	LEON_DEFAULT,
	LEON_HURT,
	LEON_HURT_SIDE_PACK,
	LEON_SPECIAL_1,
	LEON_SPECIAL_2,
	CLAIRE_DEFAULT,
	CLAIRE_NO_JACKET,
	CLAIRE_NO_JACKET_AND_SIDE_PACK,
	CLAIRE_SPECIAL,
	SHERRY,
	SHERRY_CLAIRE_JACKET,
	ADA,
	ADA_HURT,
	CHRIS,
	CHRIS_SHOTGUN,
	HUNK,
	TOFU,
	CHIEF_IRONS,
	BEN_BERTOLUCCI_SCARED,
	BEN_BERTOLUCCI,
	ANNETTE_BIRKIN_1,
	ANNETTE_BIRKIN_2,
	KENDO,
	ZOMBIE_1,
	ZOMBIE_2,
	ZOMBIE_GIRL,
	ZOMBIE_POLICE,
	ZOMBIE_PATIENT,
	ZOMBIE_SCIENTIST,
	ZOMBIE_SKINNED,
	ZOMBIE_ARMS,
	MR_X,
	TYRANT,
	TYRANT_ARM,
	IVY,
	IVY_ARM,
	GIANT_MOTH,
	LARVAE,
	LAB_MECHANICAL_ARM,
	LAB_DEVICE,
	MARVIN,
	KATHERINE,
	BRAD_VICKERS,
	DOG,
	CROW,
	LICKER_RED,
	ALLIGATOR,
	LICKER_GREY,
	SPIDER,
	SPIDER_BABY,
	MAGGOTS_BABY,
	MAGGOTS,
	COCKROACH,
	BIRKIN_1,
	BIRKIN_2,
	BIRKIN_3,
	BIRKIN_4,
	// Fix: First clut is wider than others 2
	// BIRKIN_5,
	BIRKIN_PROJECTILE_1,
	BIRKIN_PROJECTILE_2,
	IVY_PURPLE,

	MODEL_COUNT,
	UNKNOWN = 255
};

struct ModelIdMapping
{
	const char* filename;
	ModelId id;
};

constexpr ModelIdMapping modelFileTable[MODEL_COUNT] = {
	// { "PL00.PLD", LEON_DEFAULT},
	{ "EM050.EMD", LEON_DEFAULT },
	{ "PL04.PLD", LEON_HURT },
	{ "PL06.PLD", LEON_HURT_SIDE_PACK },
	{ "PL08.PLD", LEON_SPECIAL_1 },
	{ "PL0A.PLD", LEON_SPECIAL_2 },
	{ "PL01.PLD", CLAIRE_DEFAULT },
	{ "PL05.PLD", CLAIRE_NO_JACKET },
	{ "PL07.PLD", CLAIRE_NO_JACKET_AND_SIDE_PACK},
	{ "PL09.PLD", CLAIRE_SPECIAL },
	{ "EM045.EMD", SHERRY },
	{ "EM04F.EMD", SHERRY_CLAIRE_JACKET },
	{ "EM041.EMD", ADA },
	{ "EM043.EMD", ADA_HURT },
	{ "PL0B.PLD", CHRIS },
	{ "PL0BCH.PLD", CHRIS_SHOTGUN },
	{ "PL0C.PLD",  HUNK },
	{ "PL0D.PLD",  TOFU },
	{ "EM040.EMD", CHIEF_IRONS },
	{ "EM044.EMD", BEN_BERTOLUCCI_SCARED },
	{ "EM046.EMD", BEN_BERTOLUCCI },
	{ "EM047.EMD", ANNETTE_BIRKIN_1 },
	{ "EM049.EMD", ANNETTE_BIRKIN_2 },
	{ "EM048.EMD", KENDO },
	{ "EM01E.EMD", ZOMBIE_1 },
	{ "EM012.EMD", ZOMBIE_2 },
	{ "EM013.EMD", ZOMBIE_GIRL },
	{ "EM010.EMD", ZOMBIE_POLICE, },
	{ "EM015.EMD", ZOMBIE_PATIENT },
	{ "EM016.EMD", ZOMBIE_SCIENTIST },
	{ "EM017.EMD", ZOMBIE_SKINNED , },
	{ "EM02D.EMD", ZOMBIE_ARMS },
	{ "EM02A.EMD", MR_X },
	{ "EM02B.EMD", TYRANT },
	{ "EM02C.EMD", TYRANT_ARM },
	{ "EM02E.EMD", IVY },
	{ "EM02F.EMD", IVY_ARM },
	{ "EM03A.EMD", GIANT_MOTH},
	{ "EM03B.EMD", LARVAE},
	{ "EM03E.EMD", LAB_MECHANICAL_ARM },
	{ "EM03F.EMD", LAB_DEVICE },
	{ "EM04A.EMD", MARVIN },
	{ "EM04B.EMD", KATHERINE},
	{ "EM011.EMD", BRAD_VICKERS },
	{ "EM020.EMD", DOG },
	{ "EM021.EMD", CROW },
	{ "EM022.EMD", LICKER_RED },
	{ "EM023.EMD", ALLIGATOR },
	{ "EM024.EMD", LICKER_GREY },
	{ "EM025.EMD", SPIDER },
	{ "EM026.EMD", SPIDER_BABY },
	{ "EM027.EMD", MAGGOTS_BABY },
	{ "EM028.EMD", MAGGOTS },
	{ "EM029.EMD", COCKROACH },
	{ "EM030.EMD", BIRKIN_1 },
	{ "EM031.EMD", BIRKIN_2 },
	{ "EM033.EMD", BIRKIN_3 },
	{ "EM034.EMD", BIRKIN_4 },
	// { "EM036.emd", BIRKIN_5 },
	{ "EM037.EMD", BIRKIN_PROJECTILE_1 },
	{ "EM038.EMD", BIRKIN_PROJECTILE_2 },
	{ "EM039.EMD", IVY_PURPLE },

	// Duplicateds
	// EM018.emd ZOMBIE_1
	// EM01F.emd ZOMBIE_1
	// EM042.emd CHIEF_IRONS
	// EM050.emd LEON two skeletons
	// EM054.emd LEON HURT two skeletons
	// EM058.emd LEON SPECIAL 1 two skeletons
	// EM05A.emd LEON SPECIAL 2 two skeletons
	// PL00CH.PLD Leon 3 animations with pistol
	// PL0A.PLD Leon 2 special
	// PL02.PLD Leon
	// PL04.PLD Leon hurt
	// PL06.PLD Leon hurt with side pack
	// PL08.PLD Leon special 1
	// EM051.emd CLAIRE two skeletons
	// EM055.emd CLAIRE NO JACKET two skeletons
	// EM059.emd CLAIRE SPECIAL two skeletons
	// PL01.PLD Claire
	// PL03.PLD Claire with side pack
	// PL05.PLD Claire no jacket
	// PL07.PLD Claire no jacket and side pack
	// PL09.PLD Claire special
	// PL0ECH.PLD Ada
	// PL0E.PLD Ada with pistol
	// PL0F.PLD Sherry
	// EM036.emd Birkin 5, wrong mesh and texture
};

constexpr const char* modelNameTable[MODEL_COUNT] = {
	"Leon",
	"Leon (hurt)",
	"Leon (hurt + side pack)",
	"Leon (special 1)",
	"Leon (special 2)",
	"Claire",
	"Claire (t-shirt)",
	"Clair (t-shirt + side pack)",
	"Claire (special 1)",
	"Sherry",
	"Sherry (Claire jacket)",
	"Ada",
	"Ada hurt",
	"Chris",
	"Chris (shotgun)",
	"Hunk",
	"Tofu",
	"Chief Irons",
	"Ben Bertolucci (scared)",
	"Ben Bertolucci",
	"Annette 1",
	"Annette 2",
	"Kendo",
	"Zombie 1",
	"Zombie 2",
	"Zombie girl",
	"Zombie police",
	"Zombie patient",
	"Zombie scientist",
	"Zombie skinned",
	"Zombie arms",
	"Myster X",
	"Tyrant",
	"Tyrant arms",
	"Ivy",
	"Ivy arm",
	"Giant moth",
	"Larvae",
	"Lab mechanical arm",
	"Lab device",
	"Marvin",
	"Katherine",
	"Brad Vickers",
	"Dog",
	"Crow",
	"Red licker",
	"Alligator",
	"Grey licker",
	"Spider",
	"Spider baby",
	"Maggots baby",
	"Maggots",
	"Cockroach",
	"Birkin 1",
	"Birkin 2",
	"Birkin 3",
	"Birkin 4",
	// "Birkin 5",
	"Birkin projectile 1",
	"Birkin projectile 2",
	"Purple ivy"
};

bool Update(Input& input, float delta);
void Render(float delta, u32 width, u32 height);
void SetupImGui();

struct EmdHeader
{
	u32 beginOffset;
	u32 offsetCount;
};

#define MAX_OFFSETS 8
#define MAX_SKELETONS 3
#define MAX_ANIMATIONS 3

#define PLD_OFFSET_COUNT 4
#define EMD_OFFSET_COUNT 8

struct Md1Header
{
	u32 length;
	u32 unknown_01;
	u32 meshCount;
};

struct Md1MeshHeader
{
	u32 triVertexOffset;
	u32 triVertexCount;
	u32 triNormalOffset;
	u32 triNormalCount;
	u32 triOffset;
	u32 triCount;
	u32 triTextureOffset;

	u32 quadVertexOffset;
	u32 quadVertexCount;
	u32 quadNormalOffset;
	u32 quadNormalCount;
	u32 quadOffset;
	u32 quadCount;
	u32 quadTextureOffset;
};

struct Md1Vertex
{
	s16 x, y, z, zero;
};

struct Md1Indices
{
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
	u16 n3, v3; // Only in quads
};

struct Md1Texture
{
	u8 u0, v0;
	u16 unknow_02;
	u8 u1, v1;
	u16 clutId;
	u8 u2, v2;
	u16 pad_00;
	u8 u3, v3;  // Only in quads
	u16 pad_01; // Only in quads
};

struct EmrHeader
{
	u16 bonesOffset;
	u16 framesOffset;
	u16 boneCount;
	u16 keyFrameSize;
};

struct EmrBoneOffset
{
	u16 meshCount;
	u16 offset;
};

struct EddHeader
{
	u16 count;
	u16 offset;
};

namespace FileExtension
{
	constexpr const char* PLD = ".PLD";
	constexpr const char* EMD = ".EMD";
	constexpr const char* TIM = ".TIM";
}

#endif