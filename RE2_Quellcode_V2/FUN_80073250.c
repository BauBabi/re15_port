/* FUN_80073250 @ 0x80073250  (Ghidra headless, raw MIPS overlay) */

void FUN_80073250(void)

{
  bool bVar1;
  byte bVar2;
  u_short uVar3;
  u_short *puVar4;
  int iVar5;
  int iVar6;
  undefined *puVar7;
  
  puVar7 = &UNK_8019bc00;
  uVar3 = GetClut(0x100,0x1eb);
  iVar5 = 8;
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
      *(undefined *)(puVar4 + -1) = (&UNK_800aae40)[iVar5];
      *(undefined *)((int)puVar4 + -1) = (&UNK_800aae41)[iVar5];
      puVar7 = puVar7 + 0x14;
      puVar4[1] = (ushort)(byte)(&UNK_800aae42)[iVar5];
      bVar2 = (&UNK_800aae43)[iVar5];
      iVar6 = iVar6 + -1;
      *puVar4 = uVar3;
      puVar4[2] = (ushort)bVar2;
      puVar4 = puVar4 + 10;
    } while (iVar6 != 0);
    bVar1 = iVar5 != 0;
    iVar5 = iVar5 + -4;
  } while (bVar1);
  DAT_800d5bfe = 0;
  DAT_800d5c19 = 0;
  DAT_800d5c18 = 0x32;
  DAT_800d5c0d = 0;
  DAT_800d5c0c = 0;
  DAT_800d5c0f = 0;
  DAT_800d5c0e = 0;
  return;
}


