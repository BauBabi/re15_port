/* FUN_80074a94 @ 0x80074a94  (Ghidra headless, raw MIPS overlay) */

void FUN_80074a94(void)

{
  byte bVar1;
  u_short uVar2;
  u_short *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined *puVar7;
  
  puVar7 = &UNK_8019ac00;
  uVar2 = GetClut(0,0x1e4);
  if (DAT_800cfc00 < 0xe) {
    DAT_800ab0f1 = 0x1e;
  }
  else {
    DAT_800ab0f1 = 0x3c;
  }
  iVar5 = 0x1c;
  do {
    iVar4 = iVar5 + -4;
    iVar6 = 2;
    puVar3 = (u_short *)(puVar7 + 0xe);
    do {
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar3 + -7) = 100;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(undefined1 *)(puVar3 + -1) = *(undefined1 *)((int)&DAT_800ab0ec + iVar5);
      *(undefined1 *)((int)puVar3 + -1) = *(undefined1 *)((int)&DAT_800ab0ec + iVar5 + 1);
      puVar7 = puVar7 + 0x14;
      puVar3[1] = (ushort)*(byte *)((int)&DAT_800ab0ee + iVar5);
      bVar1 = *(byte *)((int)&DAT_800ab0ee + iVar5 + 1);
      iVar6 = iVar6 + -1;
      *puVar3 = uVar2;
      puVar3[2] = (ushort)bVar1;
      puVar3 = puVar3 + 10;
    } while (iVar6 != 0);
    iVar5 = iVar4;
  } while (iVar4 != 0);
  DAT_800d5c24 = 0xdc;
  DAT_800d5c26 = 0x46;
  return;
}


