/* FUN_8006dea0 @ 0x8006dea0  (Ghidra headless, raw MIPS overlay) */

void FUN_8006dea0(void)

{
  bool bVar1;
  u_short uVar2;
  u_short *puVar3;
  char cVar4;
  undefined1 *puVar5;
  int iVar6;
  int iVar7;
  undefined *puVar8;
  uint uVar9;
  uint uVar10;
  
  iVar6 = 2;
  uVar10 = (uint)DAT_800d5c0a;
  puVar3 = &DAT_8019d00e;
  do {
    *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar3 + -7) = 0x66;
    *(undefined1 *)(puVar3 + -5) = 0x80;
    *(undefined1 *)((int)puVar3 + -9) = 0x80;
    *(undefined1 *)(puVar3 + -4) = 0x80;
    *(undefined1 *)(puVar3 + -1) = 0;
    *(undefined1 *)((int)puVar3 + -1) = 0;
    puVar3[1] = 0xc;
    puVar3[2] = 0xc;
    uVar2 = GetClut(0x100,0x1ec);
    *puVar3 = uVar2;
    iVar6 = iVar6 + -1;
    puVar3 = puVar3 + 10;
  } while (iVar6 != 0);
  puVar8 = &UNK_8019d028;
  iVar6 = 2;
  cVar4 = '\x0e';
  do {
    iVar6 = iVar6 + -1;
    iVar7 = 2;
    puVar3 = (u_short *)(puVar8 + 0xe);
    do {
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar3 + -7) = 0x66;
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(char *)(puVar3 + -1) = cVar4;
      *(undefined1 *)((int)puVar3 + -1) = 0xc;
      puVar3[1] = 0xe;
      puVar3[2] = 0xc;
      uVar2 = GetClut(0x100,0x1ec);
      *puVar3 = uVar2;
      puVar3 = puVar3 + 10;
      iVar7 = iVar7 + -1;
      puVar8 = puVar8 + 0x14;
    } while (iVar7 != 0);
    cVar4 = cVar4 + -0xe;
  } while (iVar6 != 0);
  iVar6 = 4;
  do {
    iVar7 = 2;
    puVar3 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar3 + -7) = 0x66;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(undefined1 *)(puVar3 + -1) = (&DAT_800a9b8c)[iVar6];
      *(undefined1 *)((int)puVar3 + -1) = (&DAT_800a9b8d)[iVar6];
      puVar8 = puVar8 + 0x14;
      puVar3[1] = (ushort)(byte)(&UNK_800a9b8e)[iVar6];
      iVar7 = iVar7 + -1;
      puVar3[2] = (ushort)(byte)(&UNK_800a9b8f)[iVar6];
      uVar2 = GetClut(0x100,0x1f1);
      *puVar3 = uVar2;
      puVar3 = puVar3 + 10;
    } while (iVar7 != 0);
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -4;
  } while (bVar1);
  uVar9 = (uint)(byte)(&DAT_800aaa3c)[uVar10 * 8];
  puVar5 = (&PTR_DAT_800aaa38)[uVar10 * 2] + uVar9 * 0x10;
  do {
    uVar9 = uVar9 - 1;
    iVar6 = 2;
    puVar3 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar3 + -7) = 0x66;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(undefined1 *)(puVar3 + -1) = puVar5[-0x10];
      *(undefined1 *)((int)puVar3 + -1) = puVar5[-0xf];
      puVar8 = puVar8 + 0x14;
      puVar3[1] = (ushort)(byte)puVar5[-0xe];
      iVar6 = iVar6 + -1;
      puVar3[2] = (ushort)(byte)puVar5[-0xd];
      uVar2 = GetClut(0x100,0x1f2);
      *puVar3 = uVar2;
      puVar3 = puVar3 + 10;
    } while (iVar6 != 0);
    puVar5 = puVar5 + -0x10;
  } while (uVar9 != 0);
  FUN_8006db44();
  return;
}


