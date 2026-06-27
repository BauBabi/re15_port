/* FUN_80074300 @ 0x80074300  (Ghidra headless, raw MIPS overlay) */

void FUN_80074300(void)

{
  bool bVar1;
  byte bVar2;
  u_short uVar3;
  u_short *puVar4;
  int iVar5;
  int iVar6;
  undefined *puVar7;
  
  puVar7 = &UNK_8019b800;
  uVar3 = GetClut(0,0x1e4);
  iVar5 = 0x1c;
  do {
    iVar6 = 2;
    puVar4 = (u_short *)(puVar7 + 0xe);
    do {
      *(undefined1 *)((int)puVar4 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar4 + -7) = 100;
      *(undefined1 *)(puVar4 + -5) = 0x80;
      *(undefined1 *)((int)puVar4 + -9) = 0x80;
      *(undefined1 *)(puVar4 + -4) = 0x80;
      *(undefined *)(puVar4 + -1) = (&UNK_800ab068)[iVar5];
      *(undefined *)((int)puVar4 + -1) = (&UNK_800ab069)[iVar5];
      puVar7 = puVar7 + 0x14;
      puVar4[1] = (ushort)(byte)(&UNK_800ab06a)[iVar5];
      bVar2 = (&UNK_800ab06b)[iVar5];
      iVar6 = iVar6 + -1;
      *puVar4 = uVar3;
      puVar4[2] = (ushort)bVar2;
      puVar4 = puVar4 + 10;
    } while (iVar6 != 0);
    bVar1 = iVar5 != 0;
    iVar5 = iVar5 + -4;
  } while (bVar1);
  DAT_800d5c34 = 0x3b;
  DAT_800d5c36 = 0x10;
  DAT_800d5bfe = 2;
  return;
}


