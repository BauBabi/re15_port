/* ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.
 * Die 15 Saetze mit neuem Untertitel seit v0.8.31, deren Datei am Stand des Tags v0.8.30 (Commit f3c0a7e70926) den alten Satz traegt.
 * { Raum, Nummer, stumm (1 = in der Tabelle der veralteten Aufnahmen, 0 = spielt, fast woertlich), Groesse und
 *   FNV-1a-64 der Datei am Tag, gehoert (Spracherkenner 19.11.2026), Untertitel ohne Sprechernamen }
 * Regel Stufe 7: RE15_ALTSATZ_UNTERTITEL_FREMD / RE15_ALTSATZ_AUFNAHME_FREMD (Entscheid 19.11.2026, PORT-WAHL). */
#ifndef RE15_ALTSATZ_UNTERTITEL_FREMD
#define RE15_ALTSATZ_UNTERTITEL_FREMD 1
#define RE15_ALTSATZ_AUFNAHME_FREMD 2
#endif
#ifdef RE15_ALTSATZ_ZEILEN
    { 0x3020u,  0, 1, 158142u, 0x6AEC2258B78EBBDCull, "Damn it! The door does not open!",
      "Damn it! It won't budge." },
    { 0x3020u,  1, 1, 207924u, 0x337F244BE2449C9Cull, "There's an open window. Maybe we could enter through there.",
      "There's an open window up there. We could get in that way." },
    { 0x3020u,  2, 0, 127676u, 0xE54328548FD495EEull, "Leon, give me a hand.",
      "Leon, give me a hand!" },
    { 0x30E0u,  3, 1, 311950u, 0xF6C5CC6B6703F507ull, "Yeah, Marvin is out in the hall. He can't get out because of all the monsters outside.",
      "He's back in the lobby. Some dogs got his leg... he can barely walk." },
    { 0x30E0u, 10, 1, 253994u, 0x7C5874F2DD82BF48ull, "That wound. Did those things get you? How careless.",
      "That's a bite... They got you?" },
    { 0x30E0u, 11, 1, 358762u, 0x3D648074B50817FAull, "The hall is no good. Those things broke through the main gate and got inside.",
      "The lobby's gone...! They smashed through the front glass and got inside!" },
    { 0x30E0u, 13, 1, 578700u, 0x4740AFC217827B9Dull, "The entire place is overrun, and if they're already all the way out here, the surface might just be done for.",
      "There's more of them than I thought... If they've come this far, the city's finished." },
    { 0x6030u,  2, 1, 163342u, 0xE81D4DC5371313B3ull, "She's out there fighting the monsters!",
      "Where is she now?" },
    { 0x6030u,  3, 1, 240618u, 0x047E2781D7831C1Full, "She's trying to keep the outside clear so we can escape.",
      "We got split up in the fire. One second she was right behind us..." },
    { 0x6030u,  4, 1, 105386u, 0xBE24B1216A6080DBull, "Why aren't you out there?",
      "If she's as tough as you say, she'll find another way out." },
    { 0x6030u,  5, 1, 358762u, 0x297E6D5DB07DB936ull, "She told me to stay with the kid and look after your friend here.",
      "Then I'm staying with the kid. And I'll keep an eye on your friend here." },
    { 0x6030u, 21, 0, 323096u, 0x6C5834C39CB0A2ABull, "Name's John. This is Sherry. We made it down here with a girl named Elza.",
      "Name's John. This is Sherry. We came down here with a girl named Elza." },
    { 0x6030u, 22, 1, 141794u, 0x3A0396F373171061ull, "She said she'd catch up with us.",
      "Elza's still out there...!" },
    { 0x6031u,  1, 1, 333498u, 0xBF08F12E5193E96Full, "A policeman. He's fighting off monsters. He wants us to start the train.",
      "A woman brought Marvin down here. Then she went back into the fire." },
    { 0x6031u,  2, 1,  61546u, 0x57DE620ADD17F96Cull, "I'll do it.",
      "Back? Why?" },
#endif
