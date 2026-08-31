# =============================================================================
# DARK-START-PIN (Nutzer-Report 2026-08-31: "drueckt man am Start des Spieles zu
# schnell enter, bleibt das ganze Spiel dunkel").
#
# GEMESSENER DEFEKT: der Title-Fade-in-Block in main.c lief BEDINGUNGSLOS in
# jeder Iteration der Titel-Schleife — auch in genau der Iteration, in der
# bestaetigt wurde und `mode` schon auf INGAME stand. Die Schleife wurde damit
# mit s_tfade_sub = 255 - (tblink>>1)*8 > 0 verlassen. AUSSERHALB der
# Titel-Schleife loescht den Wert keine einzige Stelle (einziger Schreiber ist
# re15_render_pc_title_fade_sub, alle sechs Aufrufer liegen in der Schleife),
# und re15_render_end_frame zeichnet ihn in JEDEM Frame als subtraktiven
# ABR2-Vollbildquad ueber 320x240 (render_pc.c) -> das ganze restliche Spiel
# wird um B abgedunkelt. Gemessen: Bildschirm-Maximum = 214 - B; bei fruehem
# Druck also pechschwarz (mittlere Helligkeit 0.00, max 0).
#
# ORIGINAL-SOLL: der Game-Init FUN_800161e0 killt den Frontend-Fade-Kanal 0
# BEDINGUNGSLOS (`jal FUN_80021764` @0x80016420, a0=0 im Delay-Slot
# @0x80016424) und loescht auf Schwarz (FUN_80021634(2,0) @0x8001642c/30).
#
# PIN: faehrt den echten Boot mit dem FRUEHESTMOEGLICHEN Titel-Ausstieg
# (RE15_TITLE_SHOT_AF=4, also tblink=4 -> ohne Fix B=239 = pechschwarz) und
# prueft die Zeile, die main.c am ERSTEN Spielframe schreibt:
#   [tfade] ingame sub=0 add=0 (Rest vom Titel: 239)
# Geprueft wird der WIRKSAME Wert (sub/add) — der, den re15_render_end_frame als
# Vollbildquad ueber das Spielbild legt. Die Klammer nennt zur Diagnose, wieviel der
# Titel hinterlassen hatte; ohne den Release-Fix stuende derselbe Wert auch vorn.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_dark_start_pin.cmake
# =============================================================================

if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "dark_start_pin: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "dark_start_pin: WORKDIR fehlt")
endif()

file(MAKE_DIRECTORY "${WORKDIR}")
file(REMOVE "${WORKDIR}/debug.log")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
            RE15_NO_INTRO=1              # Boot-FMV aus (spart ~20s; der Defekt haengt nicht daran)
            RE15_TITLE_SHOT=title.bmp    # Titel-Auto-Advance
            RE15_TITLE_SHOT_AF=4         # ... schon bei tblink=4 -> ohne Fix B=239
            RE15_INV_SHOT=exit34.bmp     # sauberer exit(0) bei Game-Frame 34
            "${RE15_PC_EXE}"
    WORKING_DIRECTORY "${WORKDIR}"
    TIMEOUT 100
    RESULT_VARIABLE rv
)
if(NOT rv EQUAL 0)
    message(FATAL_ERROR "dark_start_pin: re15_pc.exe exit=${rv} (erwartet 0)")
endif()
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "dark_start_pin: kein debug.log in ${WORKDIR}")
endif()

file(STRINGS "${WORKDIR}/debug.log" tf REGEX "\\[tfade\\] ingame")
if(NOT tf)
    message(FATAL_ERROR "dark_start_pin: keine [tfade]-Zeile — Instrumentierung fehlt oder das Spiel kam nie ins Gameplay")
endif()
list(GET tf 0 first)
if(NOT first MATCHES "sub=0 add=0")
    message(FATAL_ERROR
        "dark_start_pin: Title-Fade leckt ins Spiel: '${first}'\n"
        "  Erwartet 'sub=0 add=0'. Ein Restwert > 0 dunkelt JEDES weitere Spielbild ab\n"
        "  (subtraktiver Vollbildquad in re15_render_end_frame) — genau der Nutzer-Bug\n"
        "  'zu schnell Enter -> Spiel bleibt dunkel'.")
endif()

message(STATUS "dark_start_pin OK: ${first}")
