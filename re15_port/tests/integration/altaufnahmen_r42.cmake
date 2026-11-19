# ERZEUGT von tools/r42_namen/altaufnahmen_bauen.py - nicht von Hand aendern.
# Runde 42 Strang "namen", Stufe 6: SHA-256 der 15 veralteten Aufnahmen (Stand Tag v0.8.30, Commit f3c0a7e70926).
# Eine Datei mit genau dieser Pruefsumme behandelt der Port wie eine fehlende Datei (engine/src/altaufnahme_r42.c,
# Haken audio_pc.c re15_voice_load_clip). Die Integrationstests fragen damit den Stand der Datei im Baum ab und
# bleiben gruen, wenn der Nutzer seine neue Aufnahme abgelegt hat (dann muss die Zeile SPIELEN).
# Liste flach: je Datei vier Eintraege (Raum 4 Hex; Nummer; Pfad; SHA-256).
set(RE15_ALTAUFNAHMEN_R42
    "3020;0;synchro/STAGE3/room3020/main00.wav;349c9698b2ead89324d0becc60953769bb186f4d79dc04a637a4a72933f89da4"
    "3020;1;synchro/STAGE3/room3020/main01.wav;99b3183f1e61d78c1a51dd3fec4f1034c18f2f0672138ef74af9274841df747b"
    "3020;2;synchro/STAGE3/room3020/main02.wav;348f0398ed4d356ef1dccb282be9eaacc20061b156a5f0016cd85877985b8c00"
    "30E0;3;synchro/STAGE3/room30E0/main03.wav;4a98ebf83f74c3a47e2101e746cb74778f0ebbc633de2e7a1ea48a489dacf319"
    "30E0;10;synchro/STAGE3/room30E0/main10.wav;8d9dada80ff81248358a34e69e99baea5a1ae3dca806d36866845102e28ff512"
    "30E0;11;synchro/STAGE3/room30E0/main11.wav;93174b23bb0ccb482ca5fe54d7eecc2535988664ae09a65c80c4d6b5be639f41"
    "30E0;13;synchro/STAGE3/room30E0/main13.wav;5cedb7670d136ff0efda1ea329c28e01f444f71eeb35f025108691d5056751b2"
    "6030;2;synchro/STAGE6/room6030/main02.wav;456c4d72067a8d8ed5b624dc693cae4fc3346e1b6e9817e13d497f82d2ed547a"
    "6030;3;synchro/STAGE6/room6030/main03.wav;b80b5b2f6e0c41f832116db40db4651aaf4c5b3e5ac4e9c6cd2f55010eb0d88a"
    "6030;4;synchro/STAGE6/room6030/main04.wav;353c1b42f6646d54d9b9cf89230c495e5304696395b404304464b8b4ae9d6d30"
    "6030;5;synchro/STAGE6/room6030/main05.wav;9116f63d5fb37f9359c27e45b345e5686cb6f032a78cd5a1d49001d8bc3d49d5"
    "6030;21;synchro/STAGE6/room6030/main21.wav;fcfc5bc7b91d4d7b4c7920f7de76082f5f6b56c76275b8a57c4090784361cb55"
    "6030;22;synchro/STAGE6/room6030/main22.wav;acd07c2ff8d6381103f6b8fb6c6dcece451fedb94bfde8e92f2c7f2b676c7503"
    "6031;1;synchro/STAGE6/room6031/main01.wav;9877a43effbb197eac07ede8ec05b859cac3b8b09c0e4fe8d8b2c6f0f11c4bf4"
    "6031;2;synchro/STAGE6/room6031/main02.wav;962bfc59a1fe91395cacf8e0669c8542faddca58fff0afb0257a8533624c2c08"
)
# re15_altaufnahme_stand(<out> <synchro-Wurzel> <Raum 4 Hex> <Nummer>): ALT = Datei traegt noch die alte Aufnahme,
# NEU = Datei ersetzt, FEHLT = keine Datei, KEINE = (Raum, Nummer) steht nicht in der Liste.
function(re15_altaufnahme_stand _out _synchro _raum _nr)
    set(${_out} "KEINE" PARENT_SCOPE)
    foreach(_i RANGE 0 14)
        math(EXPR _a "${_i} * 4")
        math(EXPR _b "${_a} + 1")
        math(EXPR _c "${_a} + 2")
        math(EXPR _d "${_a} + 3")
        list(GET RE15_ALTAUFNAHMEN_R42 ${_a} _r)
        list(GET RE15_ALTAUFNAHMEN_R42 ${_b} _n)
        list(GET RE15_ALTAUFNAHMEN_R42 ${_c} _p)
        list(GET RE15_ALTAUFNAHMEN_R42 ${_d} _s)
        if(_r STREQUAL "${_raum}" AND _n EQUAL ${_nr})
            string(REGEX REPLACE "^synchro/" "" _rel "${_p}")
            if(NOT EXISTS "${_synchro}/${_rel}")
                set(${_out} "FEHLT" PARENT_SCOPE)
            else()
                file(SHA256 "${_synchro}/${_rel}" _ist)
                if(_ist STREQUAL "${_s}")
                    set(${_out} "ALT" PARENT_SCOPE)
                else()
                    set(${_out} "NEU" PARENT_SCOPE)
                endif()
            endif()
            return()
        endif()
    endforeach()
endfunction()
