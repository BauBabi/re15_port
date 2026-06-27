/* FUN_80073da0 @ 0x80073da0  (Ghidra headless, raw MIPS overlay) */

void FUN_80073da0(void)

{
  bool bVar1;
  byte bVar2;
  u_short uVar3;
  undefined1 *puVar4;
  u_short *puVar5;
  int iVar6;
  int iVar7;
  undefined *puVar8;
  
  puVar8 = &DAT_8019b280;
  uVar3 = GetClut(0,0x1e4);
  iVar6 = 0x40;
  do {
    iVar7 = 2;
    puVar5 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar5 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar5 + -7) = 100;
      *(undefined1 *)(puVar5 + -5) = 0x80;
      *(undefined1 *)((int)puVar5 + -9) = 0x80;
      *(undefined1 *)(puVar5 + -4) = 0x80;
      *(undefined *)(puVar5 + -1) = (&UNK_800aaf58)[iVar6];
      *(undefined *)((int)puVar5 + -1) = (&UNK_800aaf59)[iVar6];
      puVar8 = puVar8 + 0x14;
      puVar5[1] = (ushort)(byte)(&UNK_800aaf5a)[iVar6];
      bVar2 = (&UNK_800aaf5b)[iVar6];
      iVar7 = iVar7 + -1;
      *puVar5 = uVar3;
      puVar5[2] = (ushort)bVar2;
      puVar5 = puVar5 + 10;
    } while (iVar7 != 0);
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -4;
  } while (bVar1);
  iVar6 = 2;
  puVar4 = &DAT_8019b206;
  do {
    iVar6 = iVar6 + -1;
    puVar4[-3] = 5;
    puVar4[1] = 0x2a;
    puVar4[-2] = 0x10;
    puVar4[-1] = 0x10;
    *puVar4 = 0x10;
    puVar4 = puVar4 + 0x18;
  } while (iVar6 != 0);
  DAT_800d5c28 = 7;
  DAT_800d5c2a = 0xaa;
  return;
}


