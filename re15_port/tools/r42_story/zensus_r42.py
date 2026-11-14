#!/usr/bin/env python3
"""zensus_r42.py - Runde 42 "story" (v0.8.31): Zuteilung der Nachrichten-Ids und Bank-9-Bits fuer ALLE neuen Zeilen der
Bloecke A, B und C des Story-Konzepts (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md) und
der Beweis, dass jede Zuteilung frei ist.

Die Tabelle ZEILEN unten ist die EINE Quelle: VERTRAG.md (analysis/befunde_runde42/) und die Aufnahmeliste fuer den
Nutzer werden aus ihr erzeugt (--vertrag / --aufnahme). Werte im Code: include/re15_story_r42.h.

PRUEFUNGEN (Ausgabe eingecheckt unter analysis/befunde_runde42/belege/zensus_r42.txt):
  N1 neue Id  >= Zahl der RDT-Nachrichten des Raums (Message-Block RDT+0x3C, n = off[0]/2 wie msg_common.c
     re15_msg_load_room_block; Raeume ohne Block: 0)
  N2 neue Id  nicht in PORT_BELEGT (statischer Zensus der Installationsstellen, Quelle je Eintrag)
  N3 Id < 64  (VOICE_MAX_MSG, platform/pc/src/audio_pc.c:250) - sonst laedt der Port keine Stimme
  N4 je Raum keine Id doppelt
  N5 Neutext/Neuaufnahme: die Id GIBT es schon (RDT oder Port-Zeile)
  N6 Stimme: neue Zeile -> synchro-Datei existiert noch NICHT; Neuaufnahme -> existiert (Hinweis, kein Fehler)
  B1 Bit nicht im Original-Satz (bank9_zensus.py original, Mod-Baum)
  B2 Bit nicht im Port-Satz (bank9_zensus.py port)
  B3 Bit nicht im Satz der 40-%-Disc (ausser dem bewusst uebernommenen Disc-Bit 17)
  B4 Bit = Wert in include/re15_story_r42.h, keins doppelt
  B5 Bit nicht in einem offenen r41-Zweig (ZWEIG_BITS)

Aufruf (Wurzel des Baums): python re15_port/tools/r42_story/zensus_r42.py [--vertrag | --aufnahme]
"""
import contextlib, io, os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
PSX = os.path.join(PORT, "shared_assets", "PSX")


def _haupt(name):
    """Version_40%/ und das Nutzer-synchro/ liegen im Hauptbaum (unversioniert bzw. dort befuellt); ein Arbeitsbaum
    .claude/worktrees/<x> sucht nach oben."""
    d = REPO
    for _ in range(4):
        if os.path.isdir(os.path.join(d, name)):
            return os.path.join(d, name)
        d = os.path.dirname(d)
    return os.path.join(REPO, name)


DISC40 = os.path.join(_haupt("Version_40%"), "PSX")
HAUPT = os.path.dirname(_haupt("Version_40%"))
HEADER = os.path.join(PORT, "include", "re15_story_r42.h")

# ---------------------------------------------------------------------------------------------------------------------
# Port-Zeilen, die heute schon in den betroffenen Raeumen liegen (Zensus 2026-11-14: grep der Installationsstellen
# re15_msg_install_text / story_r36 / Programm-Bytes 0x2b in engine/src, platform/pc/src, include; r41-Zweige per
# git diff HEAD...<zweig> ohne neue Ids in diesen Raeumen).
PORT_BELEGT = {
    0x2080: {42: "kanister2080_r41.c:1190 (Spur G, Kanister-Ruf)"},
    0x2081: {42: "kanister2080_r41.c:1190 (Spur G, Kanister-Ruf)"},
    0x2090: {42: "re15_gatorszene_r41.h:139", 43: "re15_gatorszene_r41.h:140",
             44: "VERTRAG R41 1.2 Spur F 42-46", 45: "VERTRAG R41 1.2 Spur F 42-46", 46: "VERTRAG R41 1.2 Spur F 42-46"},
    0x2091: {42: "re15_gatorszene_r41.h:139", 43: "re15_gatorszene_r41.h:140",
             44: "VERTRAG R41 1.2 Spur F 42-46", 45: "VERTRAG R41 1.2 Spur F 42-46", 46: "VERTRAG R41 1.2 Spur F 42-46"},
    0x2070: {40: "re15_ventilkette_r41.h:267 (Spur C)"}, 0x2071: {40: "re15_ventilkette_r41.h:267 (Spur C)"},
    0x2000: {40: "re15_ventilkette_r41.h:236 (Spur C)"}, 0x2001: {40: "re15_ventilkette_r41.h:236 (Spur C)"},
    0x30C1: {i: "VERTRAG R41 1.2 ST80 30C1/6030 21-26 (vorsorglich)" for i in range(21, 27)},
    0x6030: dict([(i, "gen/story_r36_texte.inc ST80 21-26") for i in range(21, 27)]
                 + [(i, "re15_annette_r41.h:62-65 Spur AN") for i in range(27, 31)]
                 + [(31, "re15_story80_r41.h:51"), (40, "re15_story80_r41.h:52")]),
    0x5090: {i: "re15_annette_r41.h:53-56 Spur AN" for i in range(40, 44)},
}

# ---------------------------------------------------------------------------------------------------------------------
# ZEILEN: (block, gebaut, raum, id, art, sprecher, text, alt, regie, konzept)
#   art: NEU = neue Zeile mit Stimme | SYS = neue Zeile ohne Stimme (Systemtext) | NEUAUF = vorhandene Zeile, neuer Text,
#        neu aufnehmen | UMTEXT = vorhandener Systemtext, neuer Wortlaut (ohne Stimme)
#   gebaut: "v0.8.31" | "v0.8.32" (F-24: v0.8.31 = Block A + B + Texte aus C)
V31, V32 = "v0.8.31", "v0.8.32"
ZEILEN = [
    # ---- Block B: 2080 Leon (L1, K-01, K-07) ----
    ("B", V31, 0x2080, 43, "NEU", "Leon", "Thanks, Marvin. I thought I was gonna end up as dinner.", "", "erleichtert, noch ausser Atem; direkt nach dem Alligator", "L1"),
    ("B", V31, 0x2080, 44, "NEU", "Leon", "What brought you here, anyway?", "", "neugierig, locker", "L1"),
    ("B", V31, 0x2080, 45, "NEU", "Marvin", "Hard to miss. The whole place was shaking.", "", "trocken, ein Hauch Humor", "L1"),
    ("B", V31, 0x2080, 46, "NEU", "Leon", "Yeah... right. Thanks again.", "", "kurz, etwas verlegen-dankbar", "L1"),
    ("B", V31, 0x2080, 47, "NEU", "Marvin", "Go on ahead, Leon. I'll wait here and watch your back.", "", "ruhig, bestimmt, wie ein Vorgesetzter", "L1 K-07"),
    ("B", V31, 0x2080, 48, "NEU", "Marvin", "And I'll keep an eye out for that little girl.", "", "fuersorglich, etwas leiser", "L1 K-07"),
    ("B", V31, 0x2080, 52, "SYS", "System", "Next to the computer... there's a keyhole.", "", "beim Ablassen des Wassers", "L1 K-01/K-04"),
    ("B", V31, 0x2080, 53, "SYS", "System", "The gate is locked. It needs a key.", "", "Tor ohne Schluessel", "L1"),
    ("B", V31, 0x2080, 54, "SYS", "System", "Leon used the key.", "", "Tor mit Schluessel, danach die Szene", "L1"),
    # ---- Block B: 2081 Elza (E1, F-03 ja, K-07) ----
    ("B", V31, 0x2081, 43, "NEU", "Elza", "Thanks, John. I thought I was gonna end up as dinner.", "", "erleichtert, noch ausser Atem; direkt nach dem Alligator", "E1"),
    ("B", V31, 0x2081, 44, "NEU", "Elza", "What brought you here, anyway?", "", "neugierig, locker", "E1"),
    ("B", V31, 0x2081, 45, "NEU", "John", "Hard to miss. The whole place was shaking.", "", "trocken, ein Hauch Humor", "E1"),
    ("B", V31, 0x2081, 46, "NEU", "Elza", "Well... thanks again.", "", "kurz, ehrlich dankbar", "E1"),
    ("B", V31, 0x2081, 47, "NEU", "Sherry", "Elza, are you okay? I heard some really loud noises!", "", "besorgt, kommt die Treppe hoch gelaufen", "E1"),
    ("B", V31, 0x2081, 48, "NEU", "Elza", "I'm fine, Sherry. Thanks.", "", "beruhigend, warm", "E1"),
    ("B", V31, 0x2081, 49, "NEU", "John", "So you're Sherry. Stick close, kid.", "", "freundlich; John sieht Sherry zum ersten Mal (F-03)", "E1 F-03"),
    ("B", V31, 0x2081, 50, "NEU", "John", "Elza, go on ahead. I'll wait here and look after Sherry until you've cleared the way.", "", "ruhig, verantwortungsvoll", "E1 K-07"),
    ("B", V31, 0x2081, 51, "NEU", "Elza", "All right. Sherry, you stay with John.", "", "entschlossen, zu Sherry sanft", "E1 K-07"),
    ("B", V31, 0x2081, 52, "SYS", "System", "Next to the computer... there's a keyhole.", "", "beim Ablassen des Wassers", "E1 K-01/K-04"),
    ("B", V31, 0x2081, 53, "SYS", "System", "The gate is locked. It needs a key.", "", "Tor ohne Schluessel", "E1"),
    ("B", V31, 0x2081, 54, "SYS", "System", "Elza used the key.", "", "Tor mit Schluessel, danach die Szene", "E1"),
    # ---- Block B: 2090/2091 Uhr neben dem Schluessel (K-08) ----
    ("B", V31, 0x2090, 47, "SYS", "System", "There's something else lying next to it...", "", "beim Aufheben des Schluessels, solange die Uhr noch liegt", "6b K-08"),
    ("B", V31, 0x2091, 47, "SYS", "System", "There's something else lying next to it...", "", "beim Aufheben des Schluessels, solange die Uhr noch liegt", "6b K-08"),
    # ---- Block B: 11A0 Leon (L3, K-07) + L1b ----
    ("B", V31, 0x11A0, 3, "NEU", "Marvin", "Leon, wait up!", "", "ruft von hinten, kommt angelaufen", "L3"),
    ("B", V31, 0x11A0, 4, "NEU", "Marvin", "No sign of the girl back there. And nothing's following us.", "", "noch etwas ausser Atem, sachlich", "L3"),
    ("B", V31, 0x11A0, 5, "NEU", "Leon", "Good. Let's get out of here.", "", "erleichtert, entschlossen", "L3"),
    ("B", V31, 0x11A0, 6, "NEU", "Marvin", "I'll go up first. Stay close.", "", "ruhig, Polizist im Einsatz", "L3"),
    ("B", V31, 0x11A0, 11, "SYS", "System", "Leon unlocked the door.", "", "Tuer zu 2070 von dieser Seite geoeffnet", "L1b K-02"),
    # ---- Block B: 11A1 Elza (E2, K-07) + Uhr (6b, F-26) + L1b ----
    ("B", V31, 0x11A1, 3, "NEU", "Sherry", "Elza! Wait for us!", "", "ruft, kommt angerannt", "E2"),
    ("B", V31, 0x11A1, 4, "NEU", "John", "It's clear back there. Nothing's following us.", "", "etwas ausser Atem, sachlich", "E2"),
    ("B", V31, 0x11A1, 5, "NEU", "Elza", "Good. Let's get out of here.", "", "erleichtert, entschlossen", "E2"),
    ("B", V31, 0x11A1, 6, "NEU", "Elza", "Sherry, wait. I think this is yours.", "", "nur wenn Elza die Uhr hat; freundlich, ueberreicht etwas", "6b F-26 K-08"),
    ("B", V31, 0x11A1, 7, "NEU", "Sherry", "My watch! I must have dropped it in the water...", "", "ueberrascht, gluecklich", "6b F-26"),
    ("B", V31, 0x11A1, 8, "NEU", "Elza", "The current carried it all the way down to that alligator.", "", "leicht amuesiert", "6b F-26"),
    ("B", V31, 0x11A1, 9, "NEU", "Elza", "Hold on to it. Your mom was right. It IS good luck.", "", "warm; IS betont", "6b F-26"),
    ("B", V31, 0x11A1, 10, "NEU", "John", "I'll go up first. Sherry, you're right behind me.", "", "ruhig, vaeterlich", "E2"),
    ("B", V31, 0x11A1, 11, "SYS", "System", "Elza unlocked the door.", "", "Tuer zu 2071 von dieser Seite geoeffnet", "L1b K-02"),
    # ---- Block B: L1b Korridor 2070/2071, Belohnungsraum 2040/2041 (K-02, K-03) ----
    ("B", V31, 0x2070, 1, "SYS", "System", "It's locked from the other side.", "", "Korridorseite, Tuer zu 11A0 und Tuer zu 2040", "L1b K-02/K-03"),
    ("B", V31, 0x2071, 1, "SYS", "System", "It's locked from the other side.", "", "Korridorseite, Tuer zu 11A1 und Tuer zu 2041", "L1b K-02/K-03"),
    ("B", V31, 0x2040, 6, "SYS", "System", "Leon unlocked the door.", "", "aus dem Belohnungsraum geoeffnet", "L1b K-03"),
    ("B", V31, 0x2041, 6, "SYS", "System", "Elza unlocked the door.", "", "aus dem Belohnungsraum geoeffnet", "L1b K-03"),
    # ---- Block B: 3020 (L5) ----
    ("B", V31, 0x3020, 0, "NEUAUF", "Leon", "Damn it! It won't budge.", "Leon: Dammit! the door does not open!", "ruettelt an der Tuer, genervt", "L5 Glaettung"),
    ("B", V31, 0x3020, 1, "NEUAUF", "Marvin", "There's an open window up there. We could get in that way.", "Marvin: There's an open window, maybe we could enter through there?", "zeigt nach oben, sachlich", "L5 Glaettung"),
    ("B", V31, 0x3020, 2, "NEUAUF", "Marvin", "Leon, give me a hand!", "Marvin: Leon give me an hand!", "kurz, auffordernd", "L5 Glaettung"),
    ("B", V31, 0x3020, 7, "NEU", "Marvin", "Wait here. I'll find a way to open this from the inside.", "", "vor dem Figurwechsel; zuversichtlich", "L5"),
    ("B", V31, 0x3020, 8, "NEU", "Leon", "Watch yourself, Marvin.", "", "ernst, besorgt", "L5"),
    ("B", V31, 0x3020, 9, "NEU", "Leon", "Gunshots...? Marvin!", "", "nach dem Rueckwechsel; erschrocken, Schuesse aus der Ferne", "L5 L-08"),
    # ---- Block B: 3060 (L5 Hunde, L6 Uebergabe, L7 Grund) ----
    ("B", V31, 0x3060, 11, "NEU", "Marvin", "Damn it...!", "", "gepresst; die Hunde greifen an, gleich fallen Schuesse", "L5"),
    ("B", V31, 0x3060, 12, "NEU", "Marvin", "Leon... Over here.", "", "schwach, verletzt; humpelt aus der orangen Tuer", "L5"),
    ("B", V31, 0x3060, 13, "NEU", "Leon", "Marvin! What happened?", "", "laeuft hin, besorgt", "L5"),
    ("B", V31, 0x3060, 14, "NEU", "Marvin", "Dogs. One of them got my leg.", "", "Schmerz, zusammengebissen", "L5"),
    ("B", V31, 0x3060, 15, "NEU", "Marvin", "I couldn't get that door open for you. But I found this.", "", "entschuldigend, reicht etwas", "L6"),
    ("B", V31, 0x3060, 16, "SYS", "System", "You've received the Green Keycard.", "", "", "L6"),
    ("B", V31, 0x3060, 17, "NEU", "Marvin", "And this. A floor plan of the plant.", "", "reicht noch etwas", "L6"),
    ("B", V31, 0x3060, 18, "SYS", "System", "You've received the factory map.", "", "", "L6"),
    ("B", V31, 0x3060, 19, "NEU", "Marvin", "Found this, too. You'll get more use out of it than me.", "", "nur wenn Marvin den Super Redhawk hat; trocken", "L6 F-06"),
    ("B", V31, 0x3060, 20, "SYS", "System", "You've received the Super Redhawk.", "", "nur mit Redhawk (Ergaenzung analog L6)", "L6 F-06"),
    ("B", V31, 0x3060, 21, "NEU", "Leon", "A keycard... This should open the door in the loading area.", "", "nachdenklich, schaut die Karte an", "L6"),
    ("B", V31, 0x3060, 22, "NEU", "Marvin", "Go on. With this leg, I'd only slow you down.", "", "muede, aber bestimmt", "L7"),
    ("B", V31, 0x3060, 23, "NEU", "Leon", "I'm not leaving you out here.", "", "protestiert", "L7"),
    ("B", V31, 0x3060, 24, "NEU", "Marvin", "I'll make it back to the lobby. I can lock myself in and keep an eye on the front gate.", "", "ruhig, hat einen Plan", "L7"),
    ("B", V31, 0x3060, 25, "NEU", "Marvin", "If Ada turns up, I'll send her your way.", "", "beilaeufig", "L7"),
    ("B", V31, 0x3060, 26, "NEU", "Leon", "Hang in there. I'll come back for you.", "", "Versprechen, ernst", "L7"),
    ("B", V31, 0x3060, 27, "NEU", "Marvin", "You'd better, rookie.", "", "kleines Laecheln in der Stimme", "L7"),
    # ---- Block B: 3070 (L10) ----
    ("B", V31, 0x3070, 6, "NEU", "Leon", "That came from the storage room... Ada?", "", "nach Birkins Flucht; lauscht, unsicher", "L10 F-08"),
    # ---- Block B: 30E0 (L7, L8, L10) ----
    ("B", V31, 0x30E0, 3, "NEUAUF", "Leon", "He's back in the lobby. Some dogs got his leg... he can barely walk.", "Leon: Y-yeah, Marvin is out in the hall. He can't get out because of all the monsters outside...", "besorgt", "L7"),
    ("B", V31, 0x30E0, 16, "NEU", "Leon", "That came from the lobby... Marvin!", "", "nach dem Glasbruch; alarmiert", "L8"),
    ("B", V31, 0x30E0, 10, "NEUAUF", "Leon", "That's a bite... They got you?", "Leon: That wound... Did those things get you? How careless...", "erschrocken, leise", "L8"),
    ("B", V31, 0x30E0, 11, "NEUAUF", "Marvin", "The lobby's gone...! They smashed through the front glass and got inside!", "Marvin: The hall is no good...! Those things broke through the main gate and got inside!", "keuchend, gebissen, Panik", "L8"),
    ("B", V31, 0x30E0, 13, "NEUAUF", "Marvin", "There's more of them than I thought... If they've come this far, the city's finished.", "Marvin: The entire place is overrun! And if they're already all the way out here... the surface might just be done for...", "erschoepft, hoffnungslos", "L8"),
    ("B", V31, 0x30E0, 17, "NEU", "Ada", "Someone's coming.", "", "optional; leise, horcht zur Tuer", "L10 optional"),
    # ---- Block C, Texte (v0.8.31): 6030 Leon (L21), Reihenfolge der Szene sub02: 21, 2, 3, 22, 4, 5 ----
    ("C", V31, 0x6030, 21, "NEUAUF", "John", "Name's John. This is Sherry. We came down here with a girl named Elza.", "John: Name's John. This is Sherry. We made it down here with a girl named Elza.", "stellt sich vor, muede", "L21"),
    ("C", V31, 0x6030, 2, "NEUAUF", "Leon", "Where is she now?", "Sherry: She's out there fighting the monsters!  (SPRECHERWECHSEL Sherry -> Leon)", "kurz, ernst", "L21"),
    ("C", V31, 0x6030, 3, "NEUAUF", "John", "We got split up in the fire. One second she was right behind us...", "John: She's trying to keep the outside clear so we can escape.", "schuldbewusst, stockt am Ende", "L21"),
    ("C", V31, 0x6030, 22, "NEUAUF", "Sherry", "Elza's still out there...!", "John: She said she'd catch up with us.  (SPRECHERWECHSEL John -> Sherry)", "den Traenen nah", "L21"),
    ("C", V31, 0x6030, 4, "NEUAUF", "Leon", "If she's as tough as you say, she'll find another way out. But we can't wait. When that reactor goes, nobody gets out.", "Leon: Why aren't you out there?", "fest, troestend und hart zugleich", "L21"),
    ("C", V31, 0x6030, 5, "NEUAUF", "John", "Then I'm staying with the kid. And I'll keep an eye on your friend here.", "John: She told me to stay with the kid... and look after your friend here.", "entschlossen", "L21"),
    # ---- Block C, Texte (v0.8.31): 6031 Elza (E6, E9) ----
    ("C", V31, 0x6031, 1, "NEUAUF", "Sherry", "A policeman brought Marvin down here. Then he went back into the fire.", "Sherry: A policeman! He's fighting off monsters. He wants us to start the train.", "aufgeregt, erzaehlt schnell", "E6"),
    ("C", V31, 0x6031, 2, "NEUAUF", "Elza", "Back? Why?", "Elza: I'll do it!", "verwundert", "E6"),
    ("C", V31, 0x6031, 12, "NEU", "Sherry", "He said he had to find someone. He told us not to wait for him.", "", "leiser, ernst", "E6"),
    ("C", V31, 0x6031, 13, "NEU", "Elza", "...Then we won't. If that core goes, no one gets out. Let's get this train moving.", "", "kurze Pause, dann entschlossen", "E6"),
    ("C", V31, 0x6031, 14, "NEU", "Sherry", "It's moving! Elza, we did it!", "", "jubelnd; der Zug ruckt an", "E9"),
    ("C", V31, 0x6031, 15, "NEU", "John", "What the hell was that? It came from the back car!", "", "erschrocken, nach dem Krachen", "E9"),
    ("C", V31, 0x6031, 16, "NEU", "Sherry", "It's that monster... it followed us!", "", "veraengstigt", "E9"),
    ("C", V31, 0x6031, 17, "NEU", "Elza", "John, stay with Sherry and Marvin. Don't open that door for anyone but me.", "", "ruhig, befehlend", "E9"),
    # ---- Block C (v0.8.32): 4010 Leon (L13, L15, Lageplan) ----
    ("C", V32, 0x4010, 44, "NEU", "Leon", "Marvin, I've got the vaccine. Hang on... this should help.", "", "kniet bei Marvin, angespannt", "L13"),
    ("C", V32, 0x4010, 45, "NEU", "Leon", "That announcement... what's happening?", "", "verwirrt, gehetzt", "L13"),
    ("C", V32, 0x4010, 46, "NEU", "Ada", "Something's gone wrong with the reactor. If it blows, the virus goes everywhere.", "", "kontrolliert, verschweigt etwas", "L13"),
    ("C", V32, 0x4010, 35, "NEUAUF", "Ada", "Hurry to the shelter! I'll bring Marvin.", "Ada: Hurry to the shelter!", "draengend", "L13"),
    ("C", V32, 0x4010, 47, "SYS", "System", "The monitors are on. Most of the security cameras show nothing but static.", "", "mit Strom", "L15"),
    ("C", V32, 0x4010, 48, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Lageplan Forschungszentrum", "6b F-18"),
    ("C", V32, 0x4010, 49, "SYS", "System", "You've taken the research center map.", "", "", "6b F-18"),
    ("C", V32, 0x4011, 30, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Lageplan Forschungszentrum (Elza)", "6b F-18 K-05"),
    ("C", V32, 0x4011, 31, "SYS", "System", "You've taken the research center map.", "", "", "6b F-18 K-05"),
    # ---- 5060/5061 Alarm (L13, E4) ----
    ("C", V32, 0x5060, 15, "NEU", "Durchsage", "Warning. The reactor safety system has been disabled.", "", "Lautsprecher, Computerstimme, ruhig-mechanisch", "L13"),
    ("C", V32, 0x5060, 16, "NEU", "Durchsage", "Core overload in progress. Biohazard containment can no longer be maintained.", "", "Lautsprecher, Computerstimme", "L13"),
    ("C", V32, 0x5060, 17, "NEU", "Durchsage", "All personnel, proceed to the emergency shelters immediately.", "", "Lautsprecher, Computerstimme", "L13"),
    ("C", V32, 0x5060, 18, "NEU", "Leon", "The core...? Marvin... I have to get this to him. Now!", "", "Alarm, Beben; gehetzt", "L13"),
    ("C", V32, 0x5060, 19, "SYS", "System", "Current Biohazard Level: CRITICAL. The display is flashing red.", "", "", "L13"),
    ("C", V32, 0x5060, 20, "SYS", "System", "A vaccine synthesis machine. A sample of the virus must go inside.", "", "ohne Probe", "L16"),
    ("C", V32, 0x5060, 21, "SYS", "System", "Will you set the T-Virus sample in the machine?", "", "mit Probe", "L16"),
    ("C", V32, 0x5061, 15, "NEU", "Durchsage", "Warning. The reactor safety system has been disabled.", "", "dieselbe Aufnahme wie room5060/main15", "E4"),
    ("C", V32, 0x5061, 16, "NEU", "Durchsage", "Core overload in progress. Biohazard containment can no longer be maintained.", "", "dieselbe Aufnahme wie room5060/main16", "E4"),
    ("C", V32, 0x5061, 17, "NEU", "Durchsage", "All personnel, proceed to the emergency shelters immediately.", "", "dieselbe Aufnahme wie room5060/main17", "E4"),
    ("C", V32, 0x5061, 18, "NEU", "Elza", "The core...? I have to get this to Sherry. Now!", "", "Alarm, Beben; gehetzt", "E4"),
    ("C", V32, 0x5061, 19, "SYS", "System", "Current Biohazard Level: CRITICAL. The display is flashing red.", "", "", "E4"),
    ("C", V32, 0x5061, 20, "SYS", "System", "A vaccine synthesis machine. A sample of the virus must go inside.", "", "ohne Probe", "E-12"),
    ("C", V32, 0x5061, 21, "SYS", "System", "Will you set the G-Virus sample in the machine?", "", "mit Probe", "E-12"),
    # ---- 5011 / 5021 Elza (E3, E4) ----
    ("C", V32, 0x5011, 46, "NEU", "Elza", "Here, Sherry... this will help.", "", "sanft, impft Sherry", "E4"),
    ("C", V32, 0x5011, 30, "NEUAUF", "Elza", "And the fire's already spreading. I had to run through it to get back here.", "Elza: And fire's starting to spread all over the place.", "ausser Atem, durchs Feuer gerannt", "E4 optional"),
    ("C", V32, 0x5021, 2, "NEU", "Elza", "...If Sherry's been exposed to this...", "", "liest, wird blass; leise", "E3"),
    # ---- 50E0 / 50F1 Kraftraum (L14, E-11 K-06) ----
    ("C", V32, 0x50E0, 0, "NEU", "Leon", "What the hell is that thing...?", "", "die Kreatur stuerzt herab; fassungslos", "L14"),
    ("C", V32, 0x50E0, 1, "SYS", "System", "The power control console for the lower levels. Someone is slumped over it.", "", "", "L14"),
    ("C", V32, 0x50E0, 2, "SYS", "System", "There's an ID card clipped to the body. Will you take it?", "", "", "L14"),
    ("C", V32, 0x50E0, 3, "SYS", "System", "You've taken the Research Center ID Card.", "", "", "L14"),
    ("C", V32, 0x50E0, 4, "SYS", "System", "The console is wrecked. Sparks are still spitting from the panel.", "", "nach dem Kampf", "L14"),
    ("C", V32, 0x50F1, 0, "NEU", "Elza", "What the hell is that thing...?", "", "abgeleitet aus L14 (E-11 'wie bei Leon'); fassungslos", "E-11 K-06"),
    ("C", V32, 0x50F1, 1, "SYS", "System", "The power control console for the lower levels. Someone is slumped over it.", "", "", "E-11"),
    ("C", V32, 0x50F1, 2, "SYS", "System", "There's an ID card clipped to the body. Will you take it?", "", "", "E-11"),
    ("C", V32, 0x50F1, 3, "SYS", "System", "You've taken the Research Center ID Card.", "", "", "E-11"),
    ("C", V32, 0x50F1, 4, "SYS", "System", "The console is wrecked. Sparks are still spitting from the panel.", "", "nach dem Kampf", "E-11"),
    # ---- 4030/4031 Strom (L15, E-09) ----
    ("C", V32, 0x4030, 9, "SYS", "System", "The breaker hums to life. Power has been restored to the lab.", "", "", "L15"),
    ("C", V32, 0x4030, 2, "UMTEXT", "System", "The fuses are all connected. The elevator should be running now.", "The fuses are all connected. I should be able to get to some new places now.", "", "L15"),
    ("C", V32, 0x4031, 9, "SYS", "System", "The breaker hums to life. Power has been restored to the lab.", "", "", "E-09"),
    ("C", V32, 0x4031, 2, "UMTEXT", "System", "The fuses are all connected. The elevator should be running now.", "The fuses are all connected. I should be able to get to some new places now.", "", "E-09"),
    # ---- 4090/4091 Gefriertruhe (L16, E-12) ----
] + [
    ("C", V32, r, i, "SYS", "System", t, "", "", ref)
    for r, ref, virus in ((0x4090, "L16", "T-Virus"), (0x4091, "E-12", "G-Virus"))
    for i, t in ((8, "An ultra low temperature freezer. It's dead. There's no power."),
                 (9, "Will you use the Enzyme?"),
                 (10, "You've set the Enzyme in the freezer."),
                 (11, "Will you use the Research Center ID Card?"),
                 (12, "The keypanel accepted the ID card. The display reads: \"ENTER ACCESS CODE.\""),
                 (13, "\"SAMPLE TRANSFER READY.\""),
                 (14, "The robotic arm lifts a sealed case out of the freezer..."),
                 (15, "You've taken the %s." % virus))
] + [
    ("C", V32, 0x4090, 16, "NEU", "Leon", "Easy... one slip and it's all for nothing.", "", "konzentriert, haelt den Atem an", "L16"),
    ("C", V32, 0x4091, 16, "NEU", "Elza", "Easy... one slip and it's all for nothing.", "", "abgeleitet aus L16 (eine Kette fuer beide); konzentriert", "E-12"),
    ("C", V32, 0x4091, 1, "UMTEXT", "System", "The enzyme is already in place. I need to operate the machine to synthesize a sample of the G-Virus.", "... synthesize a sample of the T-Virus.", "", "E-12"),
    # ---- 5050/5051, 4080/4081, 50A0/50A1 (L17, 6b) ----
    ("C", V32, 0x5050, 5, "UMTEXT", "System", "Registration complete. Access granted: Cell Cultivation Room.", "Registration complete.", "", "L17"),
    ("C", V32, 0x5051, 5, "UMTEXT", "System", "Registration complete. Access granted: Cell Cultivation Room.", "Registration complete.", "", "E-10"),
    ("C", V32, 0x4080, 0, "UMTEXT", "System", "\"Cell Cultivation Room\" It's electronically locked. Registered staff only.", "Cell Cultivation Room\" It's electronically locked.", "", "L17"),
    ("C", V32, 0x4081, 0, "UMTEXT", "System", "\"Cell Cultivation Room\" It's electronically locked. Registered staff only.", "Cell Cultivation Room\" It's electronically locked.", "", "E-10"),
    ("C", V32, 0x50A0, 4, "SYS", "System", "The panel accepts the card, but the shutter won't move. It's locked from the hangar side.", "", "mit ID-Karte", "6b"),
    ("C", V32, 0x50A1, 4, "SYS", "System", "The panel accepts the card, but the shutter won't move. It's locked from the hangar side.", "", "mit ID-Karte", "6b"),
    # ---- 6020 Leon (L18) ----
    ("C", V32, 0x6020, 1, "NEU", "Leon", "Ada! Is Marvin...?", "", "kommt herein, besorgt", "L18"),
    ("C", V32, 0x6020, 2, "NEU", "Ada", "He's breathing. The vaccine's doing its job... for now.", "", "leise, erschoepft", "L18"),
    ("C", V32, 0x6020, 3, "NEU", "Leon", "This whole place is coming apart.", "", "Beben im Hintergrund", "L18"),
    ("C", V32, 0x6020, 4, "NEU", "Ada", "The reactor. When it goes, this shelter won't protect anyone.", "", "sachlich, ernst", "L18"),
    ("C", V32, 0x6020, 5, "NEU", "Ada", "There's an emergency train below the hangar. That's our way out.", "", "entschlossen", "L18"),
    ("C", V32, 0x6020, 6, "NEU", "Ada", "I'll take Marvin down. Go through the C-2 corridor. The hangar gate will be opened for you.", "", "gibt Anweisungen, zuegig", "L18"),
    ("C", V32, 0x6020, 7, "NEU", "Leon", "And you?", "", "misstrauisch, besorgt", "L18"),
    ("C", V32, 0x6020, 8, "NEU", "Ada", "I'll meet you in the hangar. Go!", "", "draengt ihn hinaus", "L18"),
    # ---- 6021 Elza (E5) ----
    ("C", V32, 0x6021, 1, "NEU", "John", "Smoke's getting in under the door. This place won't hold for long.", "", "hustet leicht, angespannt", "E5"),
    ("C", V32, 0x6021, 2, "NEU", "Elza", "If that core goes, nowhere down here is safe.", "", "nuechtern", "E5"),
    ("C", V32, 0x6021, 3, "NEU", "Sherry", "There's a train! Under the hangar. Dad's people used it to leave the city.", "", "eifrig, weiss etwas", "E5"),
    ("C", V32, 0x6021, 4, "NEU", "Elza", "Can you get us there?", "", "hoffnungsvoll", "E5"),
    ("C", V32, 0x6021, 5, "NEU", "Sherry", "The hangar gate only opens from the inside. But I know a way in!", "", "schon im Losrennen", "E5"),
    ("C", V32, 0x6021, 6, "NEU", "John", "Sherry, wait!", "", "ruft ihr nach", "E5"),
    ("C", V32, 0x6021, 7, "NEU", "Elza", "John, go after her. I'll meet you at the hangar.", "", "schnell, befehlend", "E5"),
    # ---- 5140/5141 Hangar-Tor (L19, E5) ----
    ("C", V32, 0x5140, 0, "NEUAUF", "Annette", "It's open. Come through, quickly.", "(Text der Disc, nie gezeigt, nie aufgenommen) It's open on this end.", "aus dem Lautsprecher, gehetzt", "L19 F-16"),
    ("C", V32, 0x5140, 1, "NEU", "Leon", "...Who's there?", "", "optional; misstrauisch", "L19 optional"),
    ("C", V32, 0x5140, 2, "SYS", "System", "The hangar shutter is sealed. It can only be opened from the hangar side.", "", "Tor vorher", "L19"),
    ("C", V32, 0x5141, 0, "NEUAUF", "Sherry", "It's open! Back here, Elza!", "(Text der Disc, nie gezeigt, nie aufgenommen) It's open on this end.", "ruft durch das Tor, stolz", "E5"),
    ("C", V32, 0x5141, 2, "SYS", "System", "The hangar shutter is sealed. It can only be opened from the hangar side.", "", "Tor vorher", "E5"),
    # ---- 50B0 Leon (L20) ----
    ("C", V32, 0x50B0, 10, "NEUAUF", "Leon", "Ada... the reactor's safety system. That's what you \"had to do,\" isn't it?", "Leon: Ada, why did you set the reactor to blow up?", "vorwurfsvoll, leise; zieht den Schluss selbst", "L20"),
    # ---- Lageplaene (6b, F-18/F-25) ----
    ("C", V32, 0x2000, 30, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Wandplan Kanalisation", "6b K-04 F-25"),
    ("C", V32, 0x2000, 31, "SYS", "System", "You've taken the drainage map.", "", "", "6b K-04 F-25"),
    ("C", V32, 0x2001, 30, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Wandplan Kanalisation", "6b K-04 F-25"),
    ("C", V32, 0x2001, 31, "SYS", "System", "You've taken the drainage map.", "", "", "6b K-04 F-25"),
    ("C", V32, 0x30C1, 30, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Wandplan Fabrik (Elza)", "6b"),
    ("C", V32, 0x30C1, 31, "SYS", "System", "You've taken the factory map.", "", "", "6b"),
    ("C", V32, 0x6010, 30, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Wandplan U-Bahn", "6b"),
    ("C", V32, 0x6010, 31, "SYS", "System", "You've taken the subway map.", "", "", "6b"),
    ("C", V32, 0x6011, 30, "SYS", "System", "There's a map on the wall. Will you take it?", "", "Wandplan U-Bahn", "6b"),
    ("C", V32, 0x6011, 31, "SYS", "System", "You've taken the subway map.", "", "", "6b"),
]

BITS = [  # (makro, bit, block, gebaut, zweck)
    ("RE15_R42_BIT_2080_SZENE", 118, "B", V31, "Nachgespraech 2080/2081 gesehen (L1/E1)"),
    ("RE15_R42_BIT_2080_TOR", 119, "B", V31, "Tor 2080/2081 mit dem Schluessel geoeffnet (K-01)"),
    ("RE15_R42_BIT_LEITER", 120, "B", V31, "Szene vor der Leiter 11A0/11A1 gesehen (K-07)"),
    ("RE15_R42_BIT_TUER_11A", 121, "B", V31, "Tuer 11A0/11A1 <-> 2070/2071 von der 11A-Seite geoeffnet (K-02)"),
    ("RE15_R42_BIT_TUER_2040", 122, "B", V31, "Tuer 2040/2041 <-> 2070/2071 aus dem Belohnungsraum geoeffnet (K-03)"),
    ("RE15_R42_BIT_3020_SCHUESSE", 123, "B", V31, "3020: Leon hoert die Schuesse (L5)"),
    ("RE15_R42_BIT_3060_UEBERGABE", 124, "B", V31, "3060: Wiedersehen + Uebergabe, oeffnet Kartentuer 3010 (L5-L7, L-10)"),
    ("RE15_R42_BIT_FINALE_ENDE", 129, "A", V31, "Finale beendet, Abspann frei (T-07)"),
    ("RE15_R42_BIT_ZUG_ELZA", 130, "C", V31, "Zugstart 6031 gesehen (E9)"),
    ("RE15_R42_BIT_ALARM", 131, "C", V32, "Alarm/Durchsage 5060/5061 (L13/E4)"),
    ("RE15_R42_BIT_IMPFUNG", 132, "C", V32, "Begleiter geimpft 4010/5011 (L13/E4)"),
    ("RE15_R42_BIT_SCHUTZRAUM", 23, "C", V32, "Schutzraum-Szene 6020/6021 (L18/E5)"),
    ("RE15_R42_BIT_HANGAR", 24, "C", V32, "Hangar-Tor 5140/5141 offen (L19/E5)"),
    ("RE15_R42_BIT_IDKARTE", 25, "C", V32, "Research Center ID Card genommen (L14/E-11)"),
    ("RE15_R42_BIT_ENZYM_DRIN", 26, "C", V32, "Enzym in der Truhe 4090/4091 (L16)"),
    ("RE15_R42_BIT_KARTE_OK", 27, "C", V32, "ID-Karte am Tastenfeld 4090/4091 (L16)"),
    ("RE15_R42_BIT_CODE_OK", 28, "C", V32, "Code eingegeben 4090/4091 (L16/L17)"),
    ("RE15_R42_BIT_PROBE", 29, "C", V32, "Virusprobe genommen 4090/4091 (L16)"),
    ("RE15_R42_BIT_PROBE_DRIN", 30, "C", V32, "Probe im Geraet 5060/5061 (L16)"),
    ("RE15_R42_BIT_ZETTEL", 31, "C", V32, "Laborzettel 40B0/40B1 genommen (L17)"),
    ("RE15_R42_BIT_DOK_P4", 32, "C", V32, "Dokument P-4 Lab Incident Report 5021 (E3)"),
    ("RE15_R42_BIT_DOK_VAKZIN", 33, "C", V32, "Dokument Vaccine Synthesis 5011 (E3)"),
    ("RE15_R42_BIT_ENZYM", 17, "C", V32, "Enzym genommen = Disc-Bit ROOM40A1 @0x00ED0 pc[18] (L16)"),
    ("RE15_R42_BIT_PLAN_KANAL", 34, "C", V32, "Lageplan Kanalisation 2000/2001 (6b)"),
    ("RE15_R42_BIT_PLAN_FABRIK", 35, "B/C", V31, "Lageplan Fabrik: Leon Uebergabe 3060 (L6, v0.8.31), Elza 30C1 (v0.8.32)"),
    ("RE15_R42_BIT_PLAN_FORSCHUNG", 36, "C", V32, "Lageplan Forschungszentrum 4010/4011 (6b)"),
    ("RE15_R42_BIT_PLAN_UBAHN", 37, "C", V32, "Lageplan U-Bahn 6010/6011 (6b)"),
]
DISC_BIT_OK = {17}
# Bank-9-Bits offener, noch nicht integrierter Zweige (git diff HEAD...<zweig> -- test_r36_kartenbits.c, 2026-11-14)
ZWEIG_BITS = {126: "r41/moosgang RE15_MG_BIT_GENOMMEN", 127: "r41/moosgang RE15_MG_BIT_TOR"}


def u16(d, o): return d[o] | (d[o + 1] << 8)
def u32(d, o): return struct.unpack_from("<I", d, o)[0]


def rdt_pfad(raum, basis=PSX):
    return os.path.join(basis, "STAGE%X" % (raum >> 12), "ROOM%04X.RDT" % raum)


def rdt_nachrichten(raum):
    """Zahl der RDT-Nachrichten wie msg_common.c re15_msg_load_room_block (n = off[0]/2), Texte dazu."""
    d = open(rdt_pfad(raum), "rb").read()
    ms = u32(d, 0x3C)
    if ms == 0 or ms >= len(d):
        return 0, ms
    return u16(d, ms) // 2, ms


def stimme(raum, mid):
    return os.path.join("synchro", "STAGE%X" % (raum >> 12), "room%04X" % raum, "main%02d.wav" % mid)


def bank9_saetze():
    sys.path.insert(0, os.path.join(PORT, "tools", "r36_kartenbits"))
    sys.path.insert(0, os.path.join(PORT, "tools"))
    import bank9_zensus as Z

    def lauf(fn):
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            fn()
        return buf.getvalue()

    def belegt(txt):
        m = re.search(r"BELEGT im Original: \d+ von 256 Bits\n\s+([\d ]+)", txt)
        return set(int(x) for x in m.group(1).split())
    orig = belegt(lauf(Z.teil_original))
    alt = Z.PSX
    Z.PSX = DISC40
    disc = belegt(lauf(Z.teil_original))
    Z.PSX = alt
    ptxt = lauf(Z.teil_port)
    # Bildbytes in gen/*_plakat.inc (Palette/Pixel des Plakats, Spur C), die der Port-Zensus als "SCD 0x21 Bank 0x09" liest
    # (gen/ventilkette_r41_plakat.inc:361 = Bit 12, :3303 = Bit 17): kein Flag-Zugriff -> nicht zaehlen.
    zeilen = [z for z in ptxt.splitlines() if "_plakat.inc:" not in z]
    # Runde 42 Stufe STAGE2 (Pruefung 1, Befund 2): die Runde-42-Bits sind seit dem Bau SELBST im Port. B2 fragt nach einer
    # FREMDEN Belegung: eine Stelle, deren Bit-Argument ein RE15_R42_*-Makro ist (SCD OP_SET ... Bit RE15_R42_BIT_x, Tabellen-
    # Liste "RE15_R42_BIT_x=n" der Datei mit dem Wrapper bit9_setzen()), ist die EIGENE Nutzung des Vertrags-Bits und wird als
    # solche ausgewiesen (EIGEN), nicht als Kollision gezaehlt. Jede andere Stelle mit demselben Wert bleibt B2.
    eigen = {}
    fremd = []
    for z in zeilen:
        m = re.search(r"Bit (RE15_R42_\w+)\s+= Bit (\d+)", z) or re.search(r"flag_(?:get|set)\(\S+, (RE15_R42_\w+)\)\s+= Bit (\d+)", z)
        if m:
            eigen.setdefault(int(m.group(2)), []).append(z.strip().split()[0])
            continue
        fremd.append(z)
    ptxt = chr(10).join(fremd)
    port = set(int(x) for x in re.findall(r"= Bit (\d+)", ptxt)) | set(int(x) for x in re.findall(r"-> Bit (\d+)", ptxt))
    datei = None
    for z in fremd:
        m = re.match(r"\s+(engine/\S+|include/\S+|platform/\S+)\s+\[", z)
        if m:
            datei = m.group(1)
        for name, wert in re.findall(r"(RE15_\w+)=(\d+)", z):
            if name.startswith("RE15_R42_"):
                eigen.setdefault(int(wert), []).append("%s (Wrapper, %s)" % (datei or "?", name))
                continue
            if name in ("RE15_R41C_STROM_BIT", "RE15_R41C_SICHERUNG_BIT", "RE15_R41C_GENERATOR_BIT"):
                continue   # Bank 3 (re15_ventilkette_r41.h:216-219), Treffer des Port-Zensus nur ueber den Dateinamen
            if name in ("RE15_S2_RIEGEL_BIT", "RE15_S2_WASSER_BIT"):
                continue   # Bank 3 (re15_story2_r42.h RE15_S2_RIEGEL_BANK / RE15_S2_WASSER_BANK = 3), Treffer nur ueber den Dateinamen
            port.add(int(wert))
    bank9_saetze.eigen = eigen
    return orig, port, disc


def pruefen():
    fehler = 0
    print("== Runde 42 Story: Zuteilung Nachrichten-Ids und Bank-9-Bits (zensus_r42.py) ==")
    print("   Baum %s; synchro auch im Hauptbaum %s; Disc %s" % (REPO, HAUPT, DISC40))
    raeume = sorted(set(z[2] for z in ZEILEN))
    n_rdt = {}
    print("\n-- RDT-Nachrichten je Raum (Mod-Baum shared_assets/PSX, Block @RDT+0x3C, n = off[0]/2) --")
    for r in raeume:
        n, ms = rdt_nachrichten(r)
        n_rdt[r] = n
        pb = PORT_BELEGT.get(r, {})
        print("  ROOM%04X  RDT 0..%-3s (Block @0x%05X)  Port belegt: %s" % (
            r, (n - 1) if n else "-", ms, ", ".join("%d (%s)" % (k, v) for k, v in sorted(pb.items())) or "-"))
    print("\n-- Zeilen --")
    gesehen = {}
    for (blk, geb, r, i, art, spr, txt, alt, regie, ref) in ZEILEN:
        msgs = []
        key = (r, i)
        if key in gesehen:
            msgs.append("N4 DOPPELT"); fehler += 1
        gesehen[key] = 1
        if not 0 <= i < 64:
            msgs.append("N3 Id >= 64 (VOICE_MAX_MSG)"); fehler += 1
        pb = PORT_BELEGT.get(r, {})
        if art in ("NEU", "SYS"):
            if i < n_rdt[r]:
                msgs.append("N1 Id liegt in den RDT-Nachrichten 0..%d" % (n_rdt[r] - 1)); fehler += 1
            if i in pb:
                msgs.append("N2 Port belegt: %s" % pb[i]); fehler += 1
        else:
            if not (i < n_rdt[r] or i in pb):
                msgs.append("N5 Id gibt es nicht"); fehler += 1
        voice = stimme(r, i)
        da = os.path.exists(os.path.join(REPO, voice)) or os.path.exists(os.path.join(HAUPT, voice))
        if art == "NEU" and da:
            msgs.append("N6 Datei liegt schon: %s" % voice); fehler += 1
        hinweis = ""
        if art == "NEUAUF":
            hinweis = "alte Aufnahme liegt" if da else "keine alte Aufnahme"
        print("  %s %s ROOM%04X %2d %-6s %-9s %s %s%s" % (
            blk, geb, r, i, art, spr, "OK" if not msgs else "FEHLER " + "; ".join(msgs),
            ("(" + hinweis + ") ") if hinweis else "", txt[:60]))
    print("\n-- Bank-9-Bits --")
    orig, port, disc = bank9_saetze()
    print("  Original (Mod-Baum): %d belegt; Port: %d belegt; 40-%%-Disc: %d belegt" % (len(orig), len(port), len(disc)))
    kopf = open(HEADER, encoding="utf-8").read()
    werte = {}
    for (m, b, blk, geb, zweck) in BITS:
        msgs = []
        mm = re.search(r"#define\s+%s\s+(\d+)" % m, kopf)
        if not mm or int(mm.group(1)) != b:
            msgs.append("B4 Kopf weicht ab"); fehler += 1
        if b in werte:
            msgs.append("B4 doppelt mit %s" % werte[b]); fehler += 1
        werte[b] = m
        if b in orig:
            msgs.append("B1 ORIGINAL belegt"); fehler += 1
        if b in port:
            msgs.append("B2 PORT belegt"); fehler += 1
        if b in disc and b not in DISC_BIT_OK:
            msgs.append("B3 Disc belegt"); fehler += 1
        if b in ZWEIG_BITS:
            msgs.append("B5 Zweig belegt: %s" % ZWEIG_BITS[b]); fehler += 1
        notiz = " (Disc-Bit, bewusst)" if b in disc and b in DISC_BIT_OK else ""
        print("  Bit %3d %-28s %-3s %s %s%s  %s" % (b, m, blk, geb, "OK" if not msgs else "FEHLER " + "; ".join(msgs),
                                                  notiz, zweck))
        st = sorted(set(getattr(bank9_saetze, "eigen", {}).get(b, [])))
        if st:
            print("          EIGEN (Port nutzt das Vertrags-Bit ueber %s): %s" % (m, ", ".join(st)))
    # Gegenprobe: jede eigene Stelle gehoert zu einem Vertrags-Bit
    for b, st in sorted(getattr(bank9_saetze, "eigen", {}).items()):
        if b not in werte:
            print("  FEHLER B6 eigene Stelle mit Bit %d ohne Vertrags-Eintrag: %s" % (b, ", ".join(sorted(set(st)))))
            fehler += 1
    frei = [b for b in range(256) if b not in orig and b not in port and b not in disc and b not in werte and b not in ZWEIG_BITS]
    print("  danach noch frei (Original, Port, Disc): %d Bits, Auszug %s" % (len(frei), frei[:24]))
    print("\nERGEBNIS: %s (%d Zeilen, %d Bits, %d Fehler)" % ("FREI" if fehler == 0 else "KOLLISION", len(ZEILEN), len(BITS), fehler))
    return fehler


def laenge(txt):
    w = len(re.findall(r"[A-Za-z0-9']+", txt))
    s = 0.6 + w / 2.6
    return "%.0f s" % max(1, round(s))


def aufnahme():
    """Markdown: je Sprecher die Zeilen mit Stimme (NEU und NEUAUF), in Spielreihenfolge der Tabelle."""
    sprecher = ["Leon", "Elza", "Marvin", "John", "Sherry", "Ada", "Annette", "Durchsage"]
    zeilen = [z for z in ZEILEN if z[4] in ("NEU", "NEUAUF")]
    for s in sprecher:
        zs = [z for z in zeilen if z[5] == s]
        if not zs:
            continue
        print("\n### %s (%d)\n" % (s, len(zs)))
        print("| Datei | Text (englisch) | Regie | Block | in v0.8.31 gebaut | Art |")
        print("|---|---|---|---|---|---|")
        for (blk, geb, r, i, art, spr, txt, alt, regie, ref) in zs:
            datei = stimme(r, i).replace(os.sep, "/")
            if art == "NEU" or alt.startswith("(Text der Disc"):
                art_s = "neu" + (" (ersetzt einen nie gezeigten Text ohne Aufnahme)" if art != "NEU" else "")
            else:
                art_s = "**neu aufnehmen** (alt: %s)" % alt.replace("|", "/")
            print("| `%s` | %s | %s, ca. %s | %s (%s) | %s | %s |" % (
                datei, txt.replace("|", "/"), regie, laenge(txt), blk, ref, "ja" if geb == V31 else "nein, v0.8.32", art_s))
    print("\nSumme je Sprecher:")
    for s in sprecher:
        zs = [z for z in zeilen if z[5] == s]
        if zs:
            n31 = sum(1 for z in zs if z[1] == V31)
            print("- %s: %d (davon %d neu aufnehmen; %d in v0.8.31 gebaut)" % (
                s, len(zs), sum(1 for z in zs if z[4] == "NEUAUF" and not z[7].startswith("(Text der Disc")), n31))
    print("- gesamt: %d" % len(zeilen))


def vertrag():
    print("| Raum | Id | Art | Sprecher | Text | Block | gebaut | Konzept |")
    print("|---|---|---|---|---|---|---|---|")
    for (blk, geb, r, i, art, spr, txt, alt, regie, ref) in sorted(ZEILEN, key=lambda z: (z[2], z[3])):
        print("| %04X | %d | %s | %s | %s | %s | %s | %s |" % (r, i, art, spr, txt.replace("|", "/"), blk, geb, ref))


if __name__ == "__main__":
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except AttributeError:
        pass
    a = sys.argv[1:]
    if "--aufnahme" in a:
        aufnahme()
    elif "--vertrag" in a:
        vertrag()
    else:
        sys.exit(1 if pruefen() else 0)
