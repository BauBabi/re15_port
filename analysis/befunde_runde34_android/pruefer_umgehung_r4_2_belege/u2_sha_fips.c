/* Gegenpruefung R4-2 (Umgehung), Y8: FIPS 180-2/-4 Beispielvektoren (NIST CSRC "SHA256.pdf" / RFC 6234 Testfaelle)
 * gegen re15_sha256_* aus asset_abgleich.c HEAD (unveraendert mituebersetzt). Ausgabe je Vektor "ok"/"FALSCH". */
#include "asset_abgleich.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int pruefe(const char *name, const unsigned char *p, size_t n, size_t wdh, size_t stueck, const char *soll)
{
    re15_sha256_t c;
    re15_sha256_start(&c);
    for (size_t w = 0; w < wdh; w++) {
        size_t o = 0;
        while (o < n) { size_t k = n - o < stueck ? n - o : stueck; re15_sha256_dazu(&c, p + o, k); o += k; }
    }
    char hex[65];
    re15_sha256_ende(&c, hex);
    int ok = strcmp(hex, soll) == 0;
    printf("%-6s %-44s %s\n", ok ? "ok" : "FALSCH", name, hex);
    return ok ? 0 : 1;
}

int main(void)
{
    int f = 0;
    const char *m448 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    const char *m896 = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    f += pruefe("leer", (const unsigned char *)"", 0, 1, 1,
                "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    f += pruefe("abc", (const unsigned char *)"abc", 3, 1, 1,
                "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    f += pruefe("448 Bit (56 B, Stueck 1)", (const unsigned char *)m448, strlen(m448), 1, 1,
                "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    f += pruefe("448 Bit (56 B, am Stueck)", (const unsigned char *)m448, strlen(m448), 1, 1000,
                "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    f += pruefe("896 Bit (112 B)", (const unsigned char *)m896, strlen(m896), 1, 7,
                "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1");
    f += pruefe("1 000 000 x 'a' (Stueck 1)", (const unsigned char *)"a", 1, 1000000, 1,
                "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
    {   /* RFC 6234 TEST4: "01234567" x 80 (640 B) */
        f += pruefe("RFC6234 TEST4 '01234567' x 80", (const unsigned char *)"0123456701234567012345670123456701234567012345670123456701234567", 64, 10, 13,
                    "594847328451bdfa85056225462cc1d867d877fb388df0ce35f25ab5562bfbb5");
    }
    {   /* 1 GiB-Vektor der NIST-Langnachrichten (SHAVS "long message"): "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno" x 16777216 */
        const char *lang = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
        f += pruefe("64 B x 16777216 (1 GiB, NIST Langnachricht)", (const unsigned char *)lang, 64, 16777216, 64,
                    "50e72a0e26442fe2552dc3938ac58658228c0cbfb1d2ca872ae435266fcd055e");
    }
    printf("ERGEBNIS: %d falsch\n", f);
    return f ? 1 : 0;
}
