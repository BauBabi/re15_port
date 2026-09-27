"""Testschranke 359 -> 360 (neuer Haken unit_elza_zweig).

local_build.sh ist ein Shell-Skript und traegt LF — hier NICHT der CRLF-Helfer.
"""
import io

B = "re15_port/tools/local_build.sh"
s = io.open(B, encoding="utf-8", newline="").read()
assert "\r\n" not in s, "local_build.sh soll LF bleiben"

paare = [
    ("#   RE15_MIN_TESTS  Standard: 359 (untere Schranke gegen eine KOLLABIERTE Suite,",
     "#   RE15_MIN_TESTS  Standard: 360 (untere Schranke gegen eine KOLLABIERTE Suite,"),
    ("#                   2026-09-27: +1 (integration_pri_masken, Runde 35) -> 359.\n"
     "#                   Gemessener Ist-Stand nach beiden Merges: 359/359.)",
     "#                   2026-09-27: +1 (integration_pri_masken, Runde 35) -> 359.\n"
     "#                   2026-09-27: +1 (unit_elza_zweig, Runde 35) -> 360.\n"
     "#                   Gemessener Ist-Stand dieses Laufs: 360/360.)"),
]
for alt, neu in paare:
    assert s.count(alt) == 1, alt[:60]
    s = s.replace(alt, neu)

assert s.count("RE15_MIN_TESTS:-359") == 2
s = s.replace("RE15_MIN_TESTS:-359", "RE15_MIN_TESTS:-360")

io.open(B, "w", encoding="utf-8", newline="").write(s)
print("Schranke 359 -> 360, LF erhalten")
