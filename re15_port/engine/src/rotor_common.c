/*
 * rotor_common.c — SHARED helicopter-rotor positional-SE math (Phase: 2026-06-09).
 *
 * Byte-true port of RE1.5 FUN_80045a64 (distance attenuation + stereo pan) and
 * FUN_80045d6c (integer azimuth). Computes the rotor (BGM SUB layer) L/R volume
 * (0..0x7f) from the camera eye/target + heli position. Compiled by BOTH ports;
 * each port's re15_audio_rotor_update() calls re15_rotor_compute_pan() and applies
 * volL/volR to its SUB layer (PSX = SPU voice L/R vol, PC = SsSeq mvol_l/r).
 *
 * This unifies what HAD DRIFTED: the PSX used this integer path, but the PC had an
 * independent copy that (1) used atan2(double) (soft-float) instead of the ATAN256
 * LUT, (2) MIRRORED the L/R channel assignment, and (3) clamped the pan index to
 * 127 instead of letting it overflow into SE_ATTEN — the original deliberately reads
 * SE_PAN's index `0x89-(rel>>3)` (up to ~137) OUT of SE_PAN into the adjacent
 * SE_ATTEN, which is why the two tables MUST stay one contiguous array.
 */
#include <stdint.h>
#include "re15_audio.h"

/* RE1.5 SE pan + distance-attenuation tables, extracted CONTIGUOUS from PSX.EXE
 * (SE_PAN @0x80074728, SE_ATTEN @0x800747a8 = +0x80). [0..127] = SE_PAN curve
 * (0x7f→0x3c); [128..255] = SE_ATTEN (0x00→0x53). One array because the pan index
 * legitimately runs past 127 into SE_ATTEN. */
static const uint8_t SE_PAN_ATTEN[256] = {
    0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
    0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,0x7d,0x7d,0x7c,0x7c,0x7c,0x7c,0x7b,0x7b,0x7a,0x7a,
    0x79,0x79,0x78,0x78,0x77,0x77,0x76,0x76,0x75,0x75,0x74,0x74,0x74,0x74,0x73,0x73,
    0x72,0x72,0x71,0x71,0x70,0x70,0x6f,0x6f,0x6e,0x6e,0x6d,0x6d,0x6c,0x6c,0x6b,0x6b,
    0x6a,0x6a,0x69,0x69,0x68,0x68,0x67,0x67,0x66,0x66,0x65,0x65,0x64,0x64,0x63,0x63,
    0x62,0x62,0x61,0x61,0x60,0x60,0x5f,0x5f,0x5e,0x5e,0x5d,0x5d,0x5c,0x5c,0x5b,0x5b,
    0x5a,0x5a,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,0x4f,0x4e,0x4d,0x4c,
    0x4b,0x4a,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,0x3f,0x3e,0x3d,0x3c,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1a,0x1b,0x1b,0x1c,0x1c,
    0x1d,0x1d,0x1e,0x1e,0x1f,0x1f,0x20,0x20,0x21,0x21,0x22,0x22,0x23,0x23,0x24,0x24,
    0x25,0x25,0x26,0x26,0x27,0x27,0x28,0x28,0x29,0x29,0x2a,0x2a,0x2b,0x2b,0x2c,0x2c,
    0x2d,0x2d,0x2e,0x2e,0x2f,0x2f,0x30,0x30,0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,
    0x35,0x35,0x36,0x36,0x37,0x37,0x38,0x38,0x39,0x39,0x3a,0x3a,0x3b,0x3b,0x3c,0x3c,
    0x3d,0x3d,0x3e,0x3e,0x3f,0x3f,0x40,0x40,0x41,0x41,0x42,0x42,0x43,0x43,0x44,0x44,
    0x45,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,
};
#define SE_PAN(i)    SE_PAN_ATTEN[(i)]          /* pan curve (may run into SE_ATTEN) */
#define SE_ATTEN(i)  SE_PAN_ATTEN[128 + (i)]    /* distance attenuation */

/* atan LUT: atan(i/256) in 4096-units (4096=360°), slope [0,1] → angle [0,0x200].
 * Faithful integer arctan (the equivalent of the BIOS `catan` FUN_80045d6c calls). */
static const unsigned short ATAN256[257] = {
      0,  3,  5,  8, 10, 13, 15, 18, 20, 23, 25, 28, 31, 33, 36, 38,
     41, 43, 46, 48, 51, 53, 56, 58, 61, 63, 66, 69, 71, 74, 76, 79,
     81, 84, 86, 89, 91, 94, 96, 99,101,104,106,108,111,113,116,118,
    121,123,126,128,131,133,136,138,140,143,145,148,150,152,155,157,
    160,162,164,167,169,172,174,176,179,181,183,186,188,190,193,195,
    197,200,202,204,207,209,211,214,216,218,220,223,225,227,229,232,
    234,236,238,241,243,245,247,249,252,254,256,258,260,262,265,267,
    269,271,273,275,277,279,282,284,286,288,290,292,294,296,298,300,
    302,304,306,308,310,312,314,316,318,320,322,324,326,328,330,332,
    334,336,338,340,342,344,346,347,349,351,353,355,357,359,360,362,
    364,366,368,370,371,373,375,377,379,380,382,384,386,387,389,391,
    393,394,396,398,399,401,403,405,406,408,410,411,413,415,416,418,
    419,421,423,424,426,428,429,431,432,434,435,437,439,440,442,443,
    445,446,448,449,451,452,454,455,457,458,460,461,463,464,466,467,
    469,470,471,473,474,476,477,479,480,481,483,484,486,487,488,490,
    491,492,494,495,496,498,499,500,502,503,504,506,507,508,509,511,
    512,
};

/* catan(z) — atan(z/4096) in 4096-units, result (-0x400,0x400). z is a Q12 ratio. */
static int rotor_catan(int z) {
    int s = 1;
    if (z < 0) { z = -z; s = -1; }
    int ang;
    if (z <= 4096) ang = ATAN256[z >> 4];                       /* slope<=1 */
    else           ang = 0x400 - ATAN256[(int)(((int64_t)4096*4096) / z) >> 4]; /* atan(z)=90°-atan(1/z) */
    return s * ang;
}

/* FUN_80045d6c — azimuth of vector (p3-p1, p4-p2) in 4096-units (4096=360°). */
static int rotor_azimuth(int p1, int p2, int p3, int p4) {
    int dx = p3 - p1;
    if (dx == 0) return ((p4 - p2) > 0) ? 0xc00 : 0x400;
    int a    = rotor_catan((int)(((int64_t)(p4 - p2) << 12) / dx));
    int base = (dx < 0) ? 0x800 : 0;
    return (base - a) & 0xfff;
}

static uint32_t rotor_isqrt(uint64_t x) {
    uint64_t r = 0, b = 1ULL << 42;
    while (b > x) b >>= 2;
    while (b) { if (x >= r + b) { x -= r + b; r = (r >> 1) + b; } else r >>= 1; b >>= 2; }
    return (uint32_t)r;
}

/* Compute the rotor L/R volume (0..0x7f) — see re15_audio.h. DISTANCE: dist(cam,heli)
 * /250 → SE_ATTEN. PAN: heli azimuth relative to the camera view direction picks a
 * SE_PAN entry for the far-side channel; the near side stays full (0x89). Both
 * channels then subtract the distance attenuation, clamped [0,0x7f]. */
void re15_rotor_compute_pan(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                            const int32_t heli_pos[3], int *out_volL, int *out_volR)
{
    /* --- distance --- */
    int64_t dx = (int64_t)cam_eye[0] - heli_pos[0];
    int64_t dy = (int64_t)cam_eye[1] - heli_pos[1];
    int64_t dz = (int64_t)cam_eye[2] - heli_pos[2];
    uint32_t r  = rotor_isqrt((uint64_t)(dx*dx + dy*dy + dz*dz));
    int di = (int)(r / 250); if (di > 0x7f) di = 0x7f;

    /* --- azimuth pan: heli bearing minus camera-facing bearing (FUN_80045a64) --- */
    int azi_h = rotor_azimuth(cam_eye[0], cam_eye[2], heli_pos[0], heli_pos[2]);
    int azi_t = rotor_azimuth(cam_eye[0], cam_eye[2], cam_tgt[0],  cam_tgt[2]);
    int azi_o = rotor_azimuth(0, 0, cam_eye[0], cam_eye[2]);
    azi_h -= azi_o;
    azi_t -= azi_o;
    unsigned rel = ((short)azi_h < (short)azi_t)
                 ? (unsigned)((azi_h + 0x1000) - azi_t)
                 : (unsigned)(azi_h - azi_t);
    int panL, panR;
    short sr = (short)rel;
    if (sr == 0 || sr == 0x1000 || sr == 0x800) {
        panL = panR = 0x89;                       /* heli dead-ahead/behind → centred */
    } else {
        int b1 = ((rel & 0x800) == 0);
        int b2 = !b1;
        int b3 = b2;
        if (((rel - 0x401) & 0xffff) < 0x7ff) b3 = b1;
        int pi = (int)((rel & 0x3ff) >> 3);       /* 0..127 */
        if (b3) pi = 0x89 - pi;                    /* may exceed 127 → reads into SE_ATTEN */
        panL = b2 ? (int)SE_PAN(pi) : 0x89;
        panR = b2 ? 0x89 : (int)SE_PAN(pi);
    }
    panL -= (int)SE_ATTEN(di); if (panL > 0x7f) panL = 0x7f; if (panL < 0) panL = 0;
    panR -= (int)SE_ATTEN(di); if (panR > 0x7f) panR = 0x7f; if (panR < 0) panR = 0;

    *out_volL = panL;
    *out_volR = panR;
}
