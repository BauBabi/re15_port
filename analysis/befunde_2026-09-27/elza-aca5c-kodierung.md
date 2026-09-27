# DAT_800aca5c ist 0/4, nicht 0/1 — und der Port testet den falschen Wert

Selbst nachgelesen am 2026-09-27, Auslieferungsstand `info/Re1.5/PSX.EXE`
(Ladebasis 0x80010000 laut Header, Datei-Offset = 0x800 + RAM - Basis).

## Der Originalwert

Beim Spielstart wird das Charakter-Byte roh an den CORE-Bank-Lader gereicht:

    800316d8: lui a0,0x800b
    800316dc: lbu a0,-13732(a0)        ; a0 = DAT_800aca5c
    800316e8: jal 0x800440c4

`FUN_800440c4` benutzt den Wert als INDEX, nicht als Flag:

    8004410c: andi v0,s2,0xff
    80044118: sll  v0,v0,1             ; *2 = Halbwort-Tabelle
    80044114: addiu v1,v1,14984        ; v1 = 0x80073a88
    8004411c: addu s1,v0,v1
    80044124: lhu  a0,0(s1)

Tabelle @0x80073a88 (Datei-Offset 0x64288), acht Halbworte:

    Index 0 -> 0x00A1      Index 4 -> 0x00A9
    Index 1 -> 0x00A3      Index 5 -> 0x00AB
    Index 2 -> 0x00A5      Index 6 -> 0x00AD
    Index 3 -> 0x00A7      Index 7 -> 0x00AF

Index 0 laedt CORE00 (Leon), Index 4 laedt CORE04 (Elza). **Das Byte traegt also 0
bzw. 4** — deshalb unterscheidet `& 4` im Original die beiden Charaktere korrekt.

## Der Defekt im Port

`re15_port/platform/pc/main.c:2995` speichert den durch vier geteilten Wert:

    re15_gameflow_new_game(ch);   /* ch = DAT_800aca5c>>2 (0=Leon,1=Elza) */

`re15_gameflow.h:32` dokumentiert das Feld folgerichtig als 0/1. Zwei Stellen testen es
aber weiterhin mit der UNGETEILTEN Maske:

    engine/src/enemy_ai_common.c:4230     if ((g_gameflow.character & 4) == 0)
    engine/src/enemy_ai_re2_zombie.c:1849 if ((g_gameflow.character & 4) == 0)

Fuer 0 und fuer 1 ist `& 4` gleich 0. Der zweite Zweig — der Elza-Zweig des
Kriech-Grab-Abwurfs — ist damit **unerreichbar**, fuer beide Charaktere.

Richtig ist entweder `character != 0` auf dem geteilten Wert, oder den Rohwert 0/4
fuehren und die Original-Maske behalten. Die zweite Form ist naeher am Original und
haelt `re15_audio_prime_core(ch ? 4 : 0)` (main.c:2999) sowie
`FUN_800440c4(DAT_800aca5c)` ohne Umrechnung zusammen.

## Was hier NICHT behauptet wird

Diese Notiz belegt die Kodierung und den Masken-Defekt. Sie sagt nichts darueber, wie die
RAUM-ID aus dem Charakter entsteht — das ist der eigentliche Auftrag der Elza-Runde.
