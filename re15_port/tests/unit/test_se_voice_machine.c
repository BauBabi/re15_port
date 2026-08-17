/* test_se_voice_machine.c — PIN auf die SE-STIMMEN-MASCHINE (Nutzer-Report 2026-08-17 #4)
 *
 *   "Der Zombie im 2F-Korridor (ROOM10D0), der nach Marvins Cutscene am Boden liegt, hat NOCH
 *    IMMER nicht die richtigen Sounds — beim Getroffenwerden, Zu-Boden-Gehen und Aufstehen."
 *
 * Die SE-ID-KETTE ist seit test_zombie_se_ab_10d0.c gepinnt und korrekt. Die Luecke lag EINE
 * Stufe tiefer: der Port hat den EDT-Record nur bis prog/tone/VAG ausgewertet und dann JEDEN
 * SE auf einem freien Mixer-Slot uebereinander gelegt. Das Original tut etwas anderes —
 * selbst disassembliert, Instruktion fuer Instruktion:
 *
 *   FUN_800453d0 (Raum-SE, Bank 3 = RDT snd1):
 *     80045478  andi s0,v1,0x1f
 *     8004547c  addiu s0,s0,-16          ; STIMME = (byte3 & 0x1f) - 0x10  -- FEST im Record
 *     800454b0  andi s1,a1,0xf           ; Prio-Nibble = byte2 & 0xf
 *     800454bc  jal 0x80045a18           ; PRIORITAETS-GATE
 *     800454c8  bne v0,zero,0x800455fc   ;   != 0 -> SE KOMPLETT VERWORFEN
 *     800454cc  andi s1,s1,0x7
 *     800454dc  sb   s1,0x800b22cc[voice]; prio[voice] = byte2 & 7
 *     80045504..58                       ; Key-On nur VORMERKEN (DAT_800b2420 + voice*0x12)
 *   FUN_80045a18 (das Gate):
 *     80045a2c  lbu v1,0x800b22cc[voice] ; laufende Prioritaet
 *     80045a30  andi a1,a1,0x7
 *     80045a34  sltu v0,a1,v1
 *     80045a44  ori v0,zero,0x1          ; neu < laufend            -> VERWERFEN
 *     80045a48  bne v1,a1,ret / 80045a4c addu v0,zero,zero ; ungleich -> ERLAUBEN
 *     80045a50-58 andi 0xff; sltiu 8; xori 1 ; Gleichstand: Nibble >= 8 -> VERWERFEN
 *   FUN_800458d4 (Frame-Pumpe): 8 Slots rueckwaerts, logische Stimme v == SPU-Stimme 16+v
 *     (iVar5 = 0x180000, je Runde -0x10000, Argument iVar5>>16 = 23..16); keyt die
 *     Vormerkung mit SsUtKeyOnV und setzt danach prio[v] = 0, sobald
 *     SpuGetKeyStatus(1 << (16+v)) == 0 (Stimme ausgeklungen).
 *
 * FOLGE, und genau das hoert der Nutzer: pro Stimme klingt IMMER NUR EIN Sample. Ein neuer SE
 * auf derselben Stimme SCHNEIDET den laufenden ab, ein leiser priorisierter faellt GANZ AUS,
 * solange die Stimme belegt ist. Die IDs feuern trotzdem — deshalb war die ID-Kette gruen und
 * es klang falsch.
 *
 * Dieser Pin friert drei Dinge ein, alle ohne Audiogeraet und ohne Klang-Urteil:
 *   A) je SE-ID 0..9 und Raum: Stimme, Prio, Prio-Nibble, VAB-Override, Extra-Layer, VAG,
 *      Tone-Attribute und die daraus berechnete effektive Tonhoehe (note2pitch2).
 *   B) die Wahrheitstabelle von FUN_80045a18 (alle vier Zweige).
 *   C) die Treffer-/Sturz-Kette des Zombies (SE 6 @0x80105cf4, SE 8 @0x801051e8/f0,
 *      SE 1 = Fall-Clip 0x0b Frame 14, SE 0 = Frame 46) durch die Maschine gefahren:
 *      welche SE tatsaechlich erklingen und auf welcher Stimme.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_vab.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    if (s <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}

/* Ein geladener snd1-Raumbank-Kontext. */
typedef struct {
    const char     *name;
    uint8_t        *raw;
    re15_rdt_t      rdt;
    re15_vab_t      vab;
    const uint8_t  *edt;
    int             ok;
} room_bank_t;

static int load_room(room_bank_t *rb, const char *base, const char *room)
{
    char path[600]; size_t n = 0;
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, room);
    rb->name = room;
    rb->raw  = slurp(path, &n);
    if (!rb->raw) return 0;
    if (re15_rdt_parse(rb->raw, n, &rb->rdt) != 0) return 0;
    if (!rb->rdt.snd_vh[1] || !rb->rdt.snd_edt[1]) return 0;
    if (re15_vab_parse(rb->rdt.snd_vh[1], (size_t)rb->rdt.snd_vh_size[1], &rb->vab) != 0) return 0;
    rb->edt = rb->rdt.snd_edt[1];
    rb->ok  = 1;
    return 1;
}

/* Effektive Tonhoehe + Sample-Laenge eines SE (Basis-Layer), exakt wie der Port sie stellt:
 * SsUtKeyOnV(..., note = tone[+6], fine = tone[+5], ...) @0x8004522c -> note2pitch2. */
static void se_sound(const room_bank_t *rb, const re15_edt_rec_t *r,
                     uint16_t *pitch, int *vag, int *samples, double *seconds)
{
    const re15_vab_tone_t *t = &rb->vab.tones[r->prog * RE15_VAB_TONES_PER_PROGRAM + r->tone];
    *pitch   = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
    *vag     = (int)t->vag_index - 1;
    *samples = 0; *seconds = 0.0;
    if (*vag >= 0 && *vag < rb->vab.vag_count) {
        *samples = (int)((rb->vab.samples[*vag].size / 16) * 28);
        double hz = 44100.0 * (double)*pitch / 4096.0;
        if (hz > 0.0) *seconds = (double)*samples / hz;
    }
}

static void dump_room(room_bank_t *rb)
{
    printf("\n== %s snd1: %d Programme, %d VAGs ==\n", rb->name, rb->vab.program_count,
           rb->vab.vag_count);
    for (int se = 0; se <= 9; se++) {
        re15_edt_rec_t r;
        if (re15_edt_decode(rb->edt, se, &r) != 0 || r.empty) { printf("  SE %d leer\n", se); continue; }
        const re15_vab_tone_t *t = &rb->vab.tones[r.prog * RE15_VAB_TONES_PER_PROGRAM + r.tone];
        uint16_t pitch; int vag, smp; double sec;
        se_sound(rb, &r, &pitch, &vag, &smp, &sec);
        printf("  SE %2d rec=%02x %02x %02x %02x | Stimme %d (SPU %2d) prio %d (nib %2d) "
               "prog %d tone %2d extra %d ovr %d | vag %2d min %3d shift %3d center %3d "
               "vol %3d pan %3d -> pitch 0x%04x = %5.0f Hz, %6d smp = %6.3f s\n",
               se, rb->edt[se*4], rb->edt[se*4+1], rb->edt[se*4+2], rb->edt[se*4+3],
               r.voice, RE15_SE_SPU_VOICE_BASE + r.voice, r.prio, r.prio_nib,
               r.prog, r.tone, r.extra, r.vab_override, vag,
               t->min_note, t->pitch_shift, t->center_note, t->vol, t->pan,
               pitch, 44100.0 * pitch / 4096.0, smp, sec);
    }
}

/* ---- C) die Maschine, exakt wie audio_pc.c sie jetzt fuehrt -------------------------- */
typedef struct {
    unsigned char prio[RE15_SE_VOICE_COUNT];
    double        busy_until[RE15_SE_VOICE_COUNT];   /* SpuGetKeyStatus-Aequivalent */
} se_machine_t;

/* Ein SE zum Zeitpunkt t (Sekunden) anstossen. Rueckgabe: 1 = erklingt, 0 = vom Gate verworfen.
 * Vor dem Gate laufen die ausgeklungenen Stimmen auf prio 0 zurueck (FUN_800458d4-Pumpe). */
static int se_fire(se_machine_t *m, const room_bank_t *rb, int se_id, double t, int *out_voice)
{
    for (int v = 0; v < RE15_SE_VOICE_COUNT; v++)
        if (t >= m->busy_until[v]) m->prio[v] = 0;

    re15_edt_rec_t r;
    if (re15_edt_decode(rb->edt, se_id, &r) != 0 || r.empty) return 0;
    *out_voice = r.voice;
    if (re15_se_prio_gate(m->prio, r.voice, r.prio_nib)) return 0;   /* FUN_80045a18 */

    uint16_t pitch; int vag, smp; double sec;
    se_sound(rb, &r, &pitch, &vag, &smp, &sec);
    if (r.voice >= 0 && r.voice < RE15_SE_VOICE_COUNT) {
        m->prio[r.voice]       = (unsigned char)r.prio;
        m->busy_until[r.voice] = t + sec;          /* Key-On schneidet das laufende Sample ab */
    }
    return 1;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    room_bank_t a = {0}, b = {0};
    CHECK(load_room(&a, base, "ROOM10D0"), "ROOM10D0 snd1-Bank geladen");
    CHECK(load_room(&b, base, "ROOM1140"), "ROOM1140 snd1-Bank geladen");
    if (!a.ok || !b.ok) { printf("\nFAILED (%d)\n", fails ? fails : 1); return 1; }

    dump_room(&a);
    dump_room(&b);

    /* ---- A) Record-Felder: Stimme/Prio sind in BEIDEN Raeumen identisch, die KLANG-Daten
     *         (VAG, Tonhoehe) sind es NICHT — das ist Datenlage, kein Port-Fehler.        */
    printf("\n-- A) Record-Felder --\n");
    {
        static const int want_voice[10] = { 4, 4, 0, 4, 5, 5, 4, 4, 5, 4 };
        /* Nibble je Raum: SE 0..8 sind byte-identisch, nur SE 9 (ausserhalb der Zombie-Kette)
         * unterscheidet sich — ROOM10D0 rec 00 01 21 14 vs. ROOM1140 rec 00 00 a4 14. */
        static const int want_nib_a[10] = { 1, 9, 1, 3,11, 9,10, 4, 2, 1 };
        static const int want_nib_b[10] = { 1, 9, 1, 3,11, 9,10, 4, 2, 4 };
        for (int se = 0; se <= 9; se++) {
            re15_edt_rec_t ra, rb2;
            re15_edt_decode(a.edt, se, &ra);
            re15_edt_decode(b.edt, se, &rb2);
            CHECK(ra.voice == want_voice[se] && rb2.voice == want_voice[se],
                  "SE %d Stimme %d in beiden Raeumen (byte3&0x1f-0x10 @0x80045478-7c)", se, want_voice[se]);
            CHECK(ra.prio_nib == want_nib_a[se] && rb2.prio_nib == want_nib_b[se],
                  "SE %d Prio-Nibble %d/%d (10D0/1140) (byte2&0xf @0x800454b0)",
                  se, want_nib_a[se], want_nib_b[se]);
            CHECK(ra.vab_override < 0 && rb2.vab_override < 0,
                  "SE %d ohne VAB-Override (byte0 bit7 @0x8004545c-68)", se);
            CHECK(ra.extra == 0 && rb2.extra == 0,
                  "SE %d ohne Extra-Layer (byte3>>5 @0x8004548c)", se);
        }
        /* Die Kollisionen, die den Klang machen: 0/1/6 teilen sich Stimme 4, 4/5/8 Stimme 5. */
        CHECK(want_voice[0] == want_voice[1] && want_voice[1] == want_voice[6],
              "SE 0, 1 und 6 teilen sich Stimme 4 -> jeder neue schneidet den laufenden ab");
        CHECK(want_voice[4] == want_voice[5] && want_voice[5] == want_voice[8],
              "SE 4, 5 und 8 teilen sich Stimme 5");
    }

    /* ---- B) Wahrheitstabelle FUN_80045a18 @0x80045a18 ---------------------------------- */
    printf("\n-- B) Prioritaets-Gate FUN_80045a18 --\n");
    {
        unsigned char prio[RE15_SE_VOICE_COUNT] = {0};
        prio[4] = 2;
        CHECK(re15_se_prio_gate(prio, 4, 1) == 1,
              "neu(1) < laufend(2) -> VERWORFEN (@0x80045a34 sltu / @0x80045a44 ori v0,1)");
        CHECK(re15_se_prio_gate(prio, 4, 3) == 0,
              "neu(3) > laufend(2) -> ERLAUBT (@0x80045a48 bne / @0x80045a4c v0=0)");
        CHECK(re15_se_prio_gate(prio, 4, 2) == 0,
              "Gleichstand, Nibble 2 < 8 -> ERLAUBT (@0x80045a50-58)");
        CHECK(re15_se_prio_gate(prio, 4, 10) == 1,
              "Gleichstand, Nibble 10 >= 8 -> VERWORFEN (Bit 3 = kein Retrigger, @0x80045a54)");
        prio[4] = 0;
        CHECK(re15_se_prio_gate(prio, 4, 9) == 0,
              "freie Stimme (prio 0), Nibble 9 -> ERLAUBT (1 != 0 @0x80045a48)");
        CHECK(re15_se_prio_gate(prio, -6, 9) == 0,
              "Direkt-Zweig (Stimme < 0) hat kein Gate (FUN_80045024 `uVar12 < 0x10`)");
    }

    /* ---- C) die gemeldete Kette durch die Maschine ------------------------------------ *
     * Frames aus test_zombie_se_ab_10d0.c (dort byte-belegt):
     *   SE 6 = Treffer, unbedingt        @0x80105cf4
     *   SE 8 = Fall-Grunzer              @0x801051e8/f0  (Knockdown-Phase 0)
     *   SE 1 = Fall-Clip 0x0b Frame 14   (Frame-Wort, FUN_8001b38c @0x8001b3b4)
     *   SE 0 = Fall-Clip 0x0b Frame 46   (Bodenaufprall)                                  */
    printf("\n-- C) Treffer -> Sturz -> Aufprall durch die Maschine --\n");
    {
        static const int   ids  [4] = { 6, 8, 1, 0 };
        static const double tsec[4] = { 0.0, 0.0, 14.0/30.0, 46.0/30.0 };
        static const char *what[4] = { "Treffer @0x80105cf4", "Fall-Grunzer @0x801051e8",
                                       "Fall f14 @0x8001b3b4", "Aufprall f46" };
        room_bank_t *rooms[2] = { &a, &b };
        int heard[2][4];
        for (int ri = 0; ri < 2; ri++) {
            se_machine_t m = {{0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}};
            printf("  [%s]\n", rooms[ri]->name);
            for (int i = 0; i < 4; i++) {
                int voice = -1;
                heard[ri][i] = se_fire(&m, rooms[ri], ids[i], tsec[i], &voice);
                printf("    t=%5.3f s  SE %d (%-24s) Stimme %d -> %s"
                       "  [Stimme frei ab %.3f s]\n",
                       tsec[i], ids[i], what[i], voice,
                       heard[ri][i] ? "ERKLINGT" : "VERWORFEN",
                       (voice >= 0 && voice < RE15_SE_VOICE_COUNT) ? m.busy_until[voice] : 0.0);
            }
        }
        for (int ri = 0; ri < 2; ri++) {
            CHECK(heard[ri][0] == 1, "%s: SE 6 (Treffer) erklingt", rooms[ri]->name);
            CHECK(heard[ri][1] == 1, "%s: SE 8 (Fall-Grunzer, eigene Stimme 5) erklingt",
                  rooms[ri]->name);
            CHECK(heard[ri][2] == 0,
                  "%s: SE 1 wird VERWORFEN — Stimme 4 fuehrt noch SE 6 mit hoeherer Prio "
                  "(FUN_80045a18). Der alte Port legte ihn zusaetzlich obendrauf.",
                  rooms[ri]->name);
            CHECK(heard[ri][3] == 1,
                  "%s: SE 0 (Aufprall) erklingt — Stimme 4 ist bis dahin ausgeklungen",
                  rooms[ri]->name);
        }
    }

    free(a.raw); free(b.raw);
    printf(fails ? "\nFAILED (%d)\n" : "\nOK\n", fails);
    return fails ? 1 : 0;
}
