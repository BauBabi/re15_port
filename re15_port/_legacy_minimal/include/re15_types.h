/**
 * @file re15_types.h
 * @brief Grundlegende Datentypen, Fixkomma-Arithmetik (Q12) und Plattform-Guards
 *
 * Definiert die gemeinsamen Typen für beide Plattformen (PSX via PSn00bSDK,
 * PC via SDL2/OpenGL). Fixkomma-Format: 20.12 (Q12) — identisch zur
 * PSX-GTE-Präzision.
 */
#ifndef RE15_TYPES_H
#define RE15_TYPES_H

#include <stdint.h>
#include <stddef.h>

/* ============================================================================
 * Plattform-Guards
 *
 * RE15_PLATFORM_PC und RE15_PLATFORM_PSX werden normalerweise über CMake
 * compile definitions gesetzt. Die folgenden #ifndef-Blöcke bieten einen
 * Fallback für den Fall, dass die Defines nicht über das Build-System
 * gesetzt wurden (z.B. bei manueller Kompilierung).
 * ========================================================================= */
#ifndef RE15_PLATFORM_PC
  #ifndef RE15_PLATFORM_PSX
    /* Fallback: Wenn weder PC noch PSX definiert ist, nehme PC an */
    #define RE15_PLATFORM_PC 1
  #endif
#endif

/* ============================================================================
 * Fixkomma-Arithmetik (Q12 — 20.12 Format)
 *
 * Identisch zur PSX-GTE-Präzision. 12 Fraktionsbits erlauben eine Auflösung
 * von 1/4096 ≈ 0.000244 pro Einheit.
 * ========================================================================= */

/** Fixkomma-Typ: signed 32-bit, 20.12 Format */
typedef int32_t fixed_q12_t;

/** Anzahl der Fraktionsbits */
#define Q12_SHIFT 12

/** Skalierungsfaktor (4096) */
#define Q12_ONE (1 << Q12_SHIFT)

/** Integer → Q12 Konvertierung */
#define Q12_FROM_INT(x) ((fixed_q12_t)((x) << Q12_SHIFT))

/** Q12 → Integer Konvertierung (Abrundung) */
#define Q12_TO_INT(x) ((int32_t)((x) >> Q12_SHIFT))

/** Q12 Multiplikation: (a * b) >> 12 */
#define Q12_MUL(a, b) ((fixed_q12_t)(((int64_t)(a) * (int64_t)(b)) >> Q12_SHIFT))

/** Q12 Division: (a << 12) / b */
#define Q12_DIV(a, b) ((fixed_q12_t)(((int64_t)(a) << Q12_SHIFT) / (b)))

/* ============================================================================
 * Allgemeine Typ-Aliase
 * ========================================================================= */

/** Boolean-Typ für C89-Kompatibilität.
 *  C23 (und neuer) definiert bool/true/false als eingebaute Schlüsselwörter —
 *  dann darf kein eigener Typedef erfolgen. Ältere Standards (inkl. der
 *  PSX-Toolchain) erhalten einen 1-Byte-bool, sofern <stdbool.h> nicht aktiv ist. */
#ifndef __cplusplus
  #if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
    #ifndef bool
      typedef uint8_t bool;
      #define true  1
      #define false 0
    #endif
  #endif
#endif

#endif /* RE15_TYPES_H */
