/* test_se_pitch.c — pins the byte-true SE tone pitch (Dossier analysis/rolltor_sound.md D1,
 * verify-korrigiert: note2pitch2 ADDIERT fine und shift @0x80056b54-5c).
 *
 * Kette: FUN_80045024 -> SsUtKeyOnV(note=tone[+6], fine=tone[+5]) @0x8004522c -> note2pitch2
 * (RE_15_Quellcode_V2/note2pitch2.c, Tabelle DAT_80077520) -> SpuVmKeyOnNow(1, pitch).
 * PSX-Pitch 0x1000 == 44100 Hz.
 *
 * Fixpunkte = die drei ROOM1130-Rolltor-Tones (RDT snd0, Tone-Entries @0x1dd4/0x1df4/0x1e14,
 * byte-verifiziert im Dossier §4.4):
 *   SE12 Klack : note=68, fine=shift=0,   center=104 -> pitch 0x1000>>3 = 512 (5512 Hz)
 *   SE10 Fahren: note=66, fine=shift=105, center=107 -> (105+105)>>3=26 -> carry+frac10;
 *                sem=1+66+60-107=20 -> LUT[8][10]=0x1A55 >>4 = 421 (~4533 Hz — das tiefe,
 *                2,3-s-Motor-Rumpeln; der alte fixe 2x-Downsample spielte 22050 Hz = 0,47 s)
 *   SE11 Stopp : note=67, fine=shift=0,   center=107 -> LUT[8][0]=0x1965 >>4 = 406 (~4371 Hz) */
#include <stdio.h>
#include "re15_vab.h"

static int fails = 0;
#define CHECK_EQ(got, want, msg) do { \
    if ((got) != (want)) { printf("FAIL: %s: got %d want %d\n", msg, (int)(got), (int)(want)); fails++; } \
} while (0)

int main(void)
{
    /* Unison: note==center, keine Feinverstimmung -> Pitch 0x1000 (1.0x = 44100 Hz). */
    CHECK_EQ(re15_vab_note2pitch2(60, 0, 60, 0), 0x1000, "unison 0x1000");
    /* -12 HT -> 0x800 (22050 Hz) — das war das ALTE konstante Verhalten des Mixers. */
    CHECK_EQ(re15_vab_note2pitch2(48, 0, 60, 0), 0x800, "-12 HT -> 0x800");
    /* Die drei Rolltor-Tones (Werte oben hergeleitet, LUT = byte-extrahierte DAT_80077520). */
    CHECK_EQ(re15_vab_note2pitch2(68, 0, 104, 0),     512, "SE12 Klack (5512 Hz)");
    CHECK_EQ(re15_vab_note2pitch2(66, 105, 107, 105), 421, "SE10 Fahren (~4533 Hz)");
    CHECK_EQ(re15_vab_note2pitch2(67, 0, 107, 0),     406, "SE11 Stopp (~4371 Hz)");
    /* Carry-Verhalten: (fine+shift)>>3 > 15 traegt EINEN Halbton (nur ein -16, note2pitch2.c). */
    CHECK_EQ(re15_vab_note2pitch2(60, 64, 60, 64), (int)(0x10F3), "carry: +16/16 HT = +1 HT");
    /* Overflow-Klemme: oct >= 1 -> Original ruft VMANAGER_OBJ_1178; Port: SPU-Ceiling 0x3FFF. */
    CHECK_EQ(re15_vab_note2pitch2(127, 0, 40, 0), 0x3FFF, "oct>=1 -> 0x3FFF ceiling");

    if (fails == 0) { printf("test_se_pitch: OK\n"); return 0; }
    printf("test_se_pitch: %d FAILURES\n", fails);
    return 1;
}
