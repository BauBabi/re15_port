/* FUN_80075fd0 @ 0x80075fd0  (Ghidra headless, raw MIPS overlay) */

void FUN_80075fd0(void)

{
  u_short uVar1;
  u_short uVar2;
  u_short *puVar3;
  int iVar4;
  int iVar5;
  char cVar6;
  undefined *puVar7;
  
  iVar5 = 2;
  puVar3 = &DAT_8019c10e;
  uVar2 = *(u_short *)
           (&DAT_800aa146 + (uint)(byte)(&DAT_800d4b68)[DAT_800d5c01 * 8 + (int)DAT_800d5c02] * 4);
  do {
    *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar3 + -7) = 0x66;
    *(undefined1 *)(puVar3 + -5) = 0x80;
    *(undefined1 *)((int)puVar3 + -9) = 0x80;
    *(undefined1 *)(puVar3 + -4) = 0x80;
    *(undefined1 *)(puVar3 + -1) = 0;
    *(undefined1 *)((int)puVar3 + -1) = 0;
    puVar3[1] = 0x100;
    puVar3[2] = 0x100 - uVar2;
    uVar1 = GetClut(0,0x1ea);
    *puVar3 = uVar1;
    iVar5 = iVar5 + -1;
    puVar3 = puVar3 + 10;
  } while (iVar5 != 0);
  puVar7 = &UNK_8019c128;
  iVar5 = 2;
  puVar3 = &DAT_8019c136;
  do {
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar3 + -7) = 0x66;
    *(undefined1 *)((int)puVar3 + -0xb) = 4;
    *(undefined1 *)(puVar3 + -5) = 0x80;
    *(undefined1 *)((int)puVar3 + -9) = 0x80;
    *(undefined1 *)(puVar3 + -4) = 0x80;
    *(undefined1 *)(puVar3 + -1) = 0;
    *(char *)((int)puVar3 + -1) = -(char)uVar2;
    puVar3[1] = 0x80;
    puVar3[2] = uVar2;
    uVar1 = GetClut(0,0x1e9);
    *puVar3 = uVar1;
    puVar3 = puVar3 + 10;
    iVar5 = iVar5 + -1;
    puVar7 = puVar7 + 0x14;
  } while (iVar5 != 0);
  iVar5 = 2;
  cVar6 = '*';
  do {
    iVar5 = iVar5 + -1;
    iVar4 = 2;
    puVar3 = (u_short *)(puVar7 + 0xe);
    do {
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar3 + -7) = 0x66;
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(char *)(puVar3 + -1) = cVar6;
      *(undefined1 *)((int)puVar3 + -1) = 0xc;
      puVar3[1] = 0xc;
      puVar3[2] = 0xd;
      uVar2 = GetClut(0x100,0x1ec);
      *puVar3 = uVar2;
      puVar3 = puVar3 + 10;
      iVar4 = iVar4 + -1;
      puVar7 = puVar7 + 0x14;
    } while (iVar4 != 0);
    cVar6 = cVar6 + -0xe;
  } while (iVar5 != 0);
  DAT_800d5c4c = 0x19;
  DAT_800d5c4e = 0x1e;
  return;
}


