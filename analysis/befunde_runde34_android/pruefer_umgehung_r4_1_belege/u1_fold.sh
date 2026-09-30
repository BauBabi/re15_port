#!/system/bin/sh
# Pruefer UMGEHUNG R4-1 (auf dem Geraet, per adb push nach /data/local/tmp): faltet der App-Speicher (FUSE ueber
# /data/media, ext4/f2fs casefold) nur ASCII - wie die Dublettenregel der Liste v2 annimmt - oder Unicode?
# Je Paar: Datei unter Name a anlegen ("A"), pruefen ob Name b sie findet, dann b schreiben ("B") und beide lesen.
# Aufruf: sh u1_fold.sh <ordner>     (Namen per printf-Oktalfolgen, damit keine Kodierung der Shell dazwischenfunkt)
D="${1:-/sdcard/Download/u1_fold}"
rm -rf "$D"; mkdir -p "$D" || { echo "mkdir $D fehlgeschlagen"; exit 1; }
probe() {   # $1 = Kennung, $2/$3 = Namen (printf-Format)
    a=$(printf "$2"); b=$(printf "$3")
    mkdir -p "$D/$1"
    printf 'A' > "$D/$1/$a"
    if [ -e "$D/$1/$b" ]; then f="GEFALTET (b findet a, Inhalt $(cat "$D/$1/$b"))"; else f="getrennt"; fi
    printf 'B' > "$D/$1/$b"
    echo "$1: $f | danach Eintraege: $(ls "$D/$1" | wc -l) | a=$(cat "$D/$1/$a") b=$(cat "$D/$1/$b")"
}
probe ascii_A_a     'A.bin'              'a.bin'
probe kontrolle_x_y 'x.bin'              'y.bin'
probe kelvin_K      'K.bin'              '\342\204\252.bin'
probe kelvin_k      'k.bin'              '\342\204\252.bin'
probe AE_ae         '\303\204.bin'       '\303\244.bin'
probe nfc_nfd       '\303\251.bin'       'e\314\201.bin'
probe sz_ss         'stra\303\237e.bin'  'strasse.bin'
probe ascii_neu     'b.bin.neu'          'B.BIN.NEU'
# NAME_MAX je Segment (Liste v2 erlaubt Segmente bis 512 B; <ziel>.neu braucht 4 B mehr)
mkdir -p "$D/lang"
n255=$(printf '%0255d' 0); n256=$(printf '%0256d' 0); n252=$(printf '%0252d' 0)
( printf 'A' > "$D/lang/$n255" ) 2>/dev/null && echo "name255: angelegt" || echo "name255: FEHLER"
( printf 'A' > "$D/lang/$n256" ) 2>/dev/null && echo "name256: angelegt" || echo "name256: FEHLER (zu lang)"
( printf 'A' > "$D/lang/$n252.neu" ) 2>/dev/null && echo "name252+.neu (256 B): angelegt" || echo "name252+.neu (256 B): FEHLER (zu lang)"
# Symlink im Speicher (Frage: kann dort ein Ziel ausserhalb liegen?)
( ln -s /data/local/tmp "$D/sym" ) 2>/dev/null && echo "symlink: angelegt ($(ls -l "$D" | grep sym))" || echo "symlink: abgelehnt"
mount | grep -E ' /storage/emulated | /data/media ' | head -3
rm -rf "$D"
echo FOLD-FERTIG
