/* FUN_80075a00 @ 0x80075a00  (Ghidra headless, raw MIPS overlay) */

void FUN_80075a00(int param_1)

{
  bool bVar1;
  u_short uVar2;
  u_short *puVar3;
  POLY_FT4 *p;
  int iVar4;
  undefined *puVar5;
  int iVar6;
  
  p = (POLY_FT4 *)&DAT_8019c000;
  iVar6 = 2;
  iVar4 = 2;
  puVar3 = &DAT_8019c20e;
  do {
    iVar4 = iVar4 + -1;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar3 + -7) = 0x66;
    puVar3[1] = 0x70;
    *(undefined1 *)((int)puVar3 + -0xb) = 4;
    *(undefined1 *)(puVar3 + -5) = 0x80;
    *(undefined1 *)((int)puVar3 + -9) = 0x80;
    *(undefined1 *)(puVar3 + -4) = 0x80;
    *(undefined1 *)(puVar3 + -1) = 0;
    *(undefined1 *)((int)puVar3 + -1) = 0;
    puVar3[2] = 0x48;
    uVar2 = GetClut(0,0x1e8);
    *puVar3 = uVar2;
    puVar3 = puVar3 + 10;
  } while (iVar4 != 0);
  puVar5 = &UNK_8019c228;
  if (param_1 != 0) {
    puVar3 = &DAT_8019c016;
    do {
      SetPolyFT4(p);
      p = p + 1;
      iVar6 = iVar6 + -1;
      *(undefined1 *)(puVar3 + -5) = 0;
      *(undefined1 *)((int)puVar3 + -9) = 0;
      *(undefined1 *)(puVar3 + -1) = 0x70;
      *(undefined1 *)((int)puVar3 + -1) = 0;
      *(undefined1 *)(puVar3 + 3) = 0;
                    /* Probable PsyQ macro: setLineF3() */
      *(undefined1 *)((int)puVar3 + 7) = 0x48;
      *(undefined1 *)(puVar3 + 7) = 0x70;
                    /* Possible PsyQ macro: setLineF3() */
      *(undefined1 *)((int)puVar3 + 0xf) = 0x48;
      *(undefined1 *)(puVar3 + -9) = 0x80;
      *(undefined1 *)((int)puVar3 + -0x11) = 0x80;
      *(undefined1 *)(puVar3 + -8) = 0x80;
      *(byte *)((int)puVar3 + -0xf) = *(byte *)((int)puVar3 + -0xf) | 2;
      uVar2 = GetClut(0,0x1e8);
      puVar3[-4] = uVar2;
      uVar2 = GetTPage(1,0,0x1c0,0x100);
      *puVar3 = uVar2;
      puVar3 = puVar3 + 0x14;
    } while (iVar6 != 0);
    iVar4 = 0x1c;
    do {
      iVar6 = 2;
      puVar3 = (u_short *)(puVar5 + 0xe);
      do {
        *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
        *(undefined1 *)((int)puVar3 + -7) = 100;
        *(undefined1 *)(puVar3 + -5) = 0x80;
        *(undefined1 *)((int)puVar3 + -9) = 0x80;
        *(undefined1 *)(puVar3 + -4) = 0x80;
        *(undefined *)(puVar3 + -1) = (&UNK_800ab19c)[iVar4];
        *(undefined *)((int)puVar3 + -1) = (&UNK_800ab19d)[iVar4];
        puVar5 = puVar5 + 0x14;
        puVar3[1] = (ushort)(byte)(&UNK_800ab19e)[iVar4];
        iVar6 = iVar6 + -1;
        puVar3[2] = (ushort)(byte)(&UNK_800ab19f)[iVar4];
        uVar2 = GetClut(0,0x1e4);
        *puVar3 = uVar2;
        puVar3 = puVar3 + 10;
      } while (iVar6 != 0);
      bVar1 = iVar4 != 0;
      iVar4 = iVar4 + -4;
    } while (bVar1);
    DAT_800dcba4 = 0x10;
    DAT_800dcba6 = 10;
    DAT_800d7834 = 0x70;
    DAT_800d783e = 0x48;
    DAT_800cbc28 = 0;
  }
  return;
}


