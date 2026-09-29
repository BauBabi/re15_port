#!/usr/bin/env bash
# Batterie der im Auftrag genannten Faelschungsarten - eigene Varianten, echtes Gate (Stand 35d25455).
# Rueckgabe je Lauf selbst abgefangen; keine Pipe um den Gate-Aufruf.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_umgehung_r2; B=analysis/befunde_runde34_android/pruefer_umgehung_r2_belege
PY=/c/Python310/python; R=$W/ref.apk; F=$W/apk/bat; S=$W/schatten; L=$W/logs/bat; mkdir -p $F $L
P07=assets/shared_assets/RE15DOOR/P07G.DO2
lauf() {   # $1 = Fall, $2 = soll, $3 = apk, [$4 = repo]
    local rc=0
    "$PY" release/apk_asset_gate.py --repo "${4:-.}" "$3" > "$L/$1.log" 2>&1 || rc=$?
    printf '%-44s soll %s ist %s %s | %s\n' "$1" "$2" "$rc" "$([[ $rc == "$2" ]] && echo ok || echo ABWEICHUNG)" \
        "$(grep -m1 -E '^      |ABBRUCH|GATE-OK' "$L/$1.log" | sed 's/^ *//' | cut -c1-150)"
    rm -f "$3"
}
f() { "$PY" $B/r2_faelschen.py $R "$@" > /dev/null || echo "FAELSCHEN FEHLGESCHLAGEN: $*"; }
# gleiche Groesse, anderer Inhalt in der GROESSTEN Datei (STR 6 MB), letztes Byte ^1 (Eintrag in sich stimmig)
cp re15_port/shared_assets/PSX/MOVIE/CAPCOM.STR $W/capcom_mod.str
$PY -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[-1]^=1; open(p,'wb').write(d)" $W/capcom_mod.str
f $F/b01.apk --daten assets/shared_assets/PSX/MOVIE/CAPCOM.STR=$W/capcom_mod.str; lauf b01_STR_6MB_letztes_Byte 1 $F/b01.apk
# gleiche Groesse, anderer Inhalt in einer BSS (Hintergrund), mittleres Byte
cp re15_port/shared_assets/PSX/BSS/ROOM1000/BG00.BSS $W/bg00_mod.bss
$PY -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x80; open(p,'wb').write(d)" $W/bg00_mod.bss
f $F/b02.apk --daten assets/shared_assets/PSX/BSS/ROOM1000/BG00.BSS=$W/bg00_mod.bss; lauf b02_BSS_mittleres_Byte 1 $F/b02.apk
# Gross/klein im ORDNER (APK + Manifest stimmig kleingeschrieben, Quelle gross)
f $F/b03.apk --umbenennen $P07=assets/shared_assets/re15door/P07G.DO2; lauf b03_Ordner_kleingeschrieben 1 $F/b03.apk
# doppelter Eintrag: der ERSTE richtig, der zweite (hinten) mit anderem Inhalt gleicher Groesse
head -c 55908 /dev/zero > $W/null55908.bin
f $F/b04.apk --doppelt $P07=$W/null55908.bin; lauf b04_doppelt_erster_richtig 1 $F/b04.apk
# Verzeichniseintrag assets/shared_assets/
f $F/b05.apk --verzeichnis assets/shared_assets/; lauf b05_Verzeichnis_shared_assets 1 $F/b05.apk
# '\' NUR im Zentralverzeichnis (LFH behaelt '/')
f $F/b06.apk --cd-backslash $P07; lauf b06_Backslash_nur_CD 1 $F/b06.apk
# Manifest CRLF bzw. Leerzeilen (Geraet: \r abgeschnitten, Leerzeilen uebersprungen -> harmlos)
f $F/b07.apk --manifest-crlf; lauf b07_Manifest_CRLF 0 $F/b07.apk
f $F/b08.apk --manifest-leerzeilen; lauf b08_Manifest_Leerzeilen 0 $F/b08.apk
# Manifest nennt eine Datei, die weder in APK noch Quelle liegt (Kopf nachgezogen)
f $F/b09.apk --manifest-zeile-plus '5\tshared_assets/RE15DOOR/GEIST.DO2'; lauf b09_Manifest_Geisterzeile 1 $F/b09.apk
# Manifest: Tab IM Pfad (zweiter Tab), Kopf nachgezogen
f $F/b10.apk --manifest-zeile-plus '5\tshared_assets/RE15DOOR/X\tY.DO2'; lauf b10_Manifest_Tab_im_Pfad 1 $F/b10.apk
# synchro-Datei ausserhalb STAGE* in der APK (+ Manifestzeile)
printf 'readme' > $W/readme6.bin
f $F/b11.apk --doppelt assets/synchro/README.md=$W/readme6.bin --manifest-zeile-plus '6\tsynchro/README.md'; lauf b11_synchro_ausserhalb_STAGE 1 $F/b11.apk
# abgeschnittene APK (letzte 1000 Bytes weg)
head -c $(( $(stat -c %s $R) - 1000 )) $R > $F/b12.apk; lauf b12_APK_abgeschnitten 2 $F/b12.apk
# leere Datei statt APK / Text statt APK
: > $F/b13.apk; lauf b13_APK_0_Byte 2 $F/b13.apk
# --- Schattenbaum: Quelle veraendert, APK = Referenz
mv $S/re15_port/shared_assets/extracted_fx $W/fx.geparkt
cp $R $F/b14.apk; lauf b14_Quellbaum_extracted_fx_fehlt 1 $F/b14.apk $S
mv $W/fx.geparkt $S/re15_port/shared_assets/extracted_fx
mkdir -p $W/door.geparkt && mv $S/re15_port/shared_assets/RE15DOOR/* $W/door.geparkt/ && mkdir -p $S/re15_port/shared_assets/RE15DOOR/leer
cp $R $F/b15.apk; lauf b15_RE15DOOR_nur_leerer_Unterordner 1 $F/b15.apk $S
rmdir $S/re15_port/shared_assets/RE15DOOR/leer && mv $W/door.geparkt/* $S/re15_port/shared_assets/RE15DOOR/ && rmdir $W/door.geparkt
G=$S/re15_port/platform/android/app/build.gradle; cp $G $W/gradle.geparkt
sed -i 's#^\(    from(new File(repoRoot, "synchro")) {\)#    from(new File(portRoot, "shared_assets/NEU")) { into "shared_assets/NEU" }\n\1#' $G
cp $R $F/b16.apk; lauf b16_build.gradle_zusaetzlicher_Baum 2 $F/b16.apk $S
cp $W/gradle.geparkt $G
diff -r $S/re15_port/shared_assets/RE15DOOR re15_port/shared_assets/RE15DOOR > /dev/null && cmp -s $G re15_port/platform/android/app/build.gradle \
  && [[ -d $S/re15_port/shared_assets/extracted_fx ]] && echo "Schatten wiederhergestellt (RE15DOOR, build.gradle, extracted_fx)"
rm -f $W/capcom_mod.str $W/bg00_mod.bss $W/null55908.bin $W/readme6.bin $W/gradle.geparkt
echo ENDE
