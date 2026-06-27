/* FUN_80074610 @ 0x80074610  (Ghidra headless, raw MIPS overlay) */

void FUN_80074610(uint param_1,int param_2)

{
  byte bVar1;
  u_short uVar2;
  u_short *puVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined *puVar7;
  
  puVar7 = &DAT_8019ba00;
  uVar2 = GetClut(0,0x1e4);
  uVar6 = 8;
  if (param_2 != 0) {
    uVar6 = 9;
  }
  if ((param_1 & 1) == 0) {
    DAT_800ab0b4 = 0x30;
  }
  else {
    DAT_800ab0b4 = 0;
  }
  if ((param_1 & 2) == 0) {
    DAT_800ab0a8 = 0xc0;
    DAT_800ab0a9 = 0x38;
  }
  else {
    DAT_800ab0a8 = 0x68;
    DAT_800ab0a9 = 0x13;
  }
  iVar4 = uVar6 * 4;
  do {
    uVar6 = uVar6 - 1;
    iVar5 = 2;
    puVar3 = (u_short *)(puVar7 + 0xe);
    do {
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar3 + -7) = 100;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(undefined1 *)(puVar3 + -1) = *(undefined1 *)((int)&DAT_800ab0a4 + iVar4);
      *(undefined1 *)((int)puVar3 + -1) = *(undefined1 *)((int)&DAT_800ab0a4 + iVar4 + 1);
      puVar7 = puVar7 + 0x14;
      puVar3[1] = (ushort)*(byte *)((int)&DAT_800ab0a6 + iVar4);
      bVar1 = *(byte *)((int)&DAT_800ab0a6 + iVar4 + 1);
      iVar5 = iVar5 + -1;
      *puVar3 = uVar2;
      puVar3[2] = (ushort)bVar1;
      puVar3 = puVar3 + 10;
    } while (iVar5 != 0);
    iVar4 = iVar4 + -4;
  } while (4 < uVar6);
  uVar2 = GetClut(0x100,0x1ea);
  iVar4 = uVar6 * 4;
  do {
    uVar6 = uVar6 - 1;
    iVar5 = 2;
    puVar3 = (u_short *)(puVar7 + 0xe);
    do {
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar3 + -7) = 100;
      if (uVar6 == DAT_800d5bff) {
        *(undefined1 *)(puVar3 + -5) = 0x40;
                    /* Possible PsyQ macro: setLineF2() */
        *(undefined1 *)((int)puVar3 + -9) = 0x40;
      }
      else {
        *(undefined1 *)(puVar3 + -5) = 0x80;
        *(undefined1 *)((int)puVar3 + -9) = 0x80;
      }
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(undefined1 *)(puVar3 + -1) = *(undefined1 *)((int)&DAT_800ab0a4 + iVar4);
      *(undefined1 *)((int)puVar3 + -1) = *(undefined1 *)((int)&DAT_800ab0a4 + iVar4 + 1);
      puVar7 = puVar7 + 0x14;
      puVar3[1] = (ushort)*(byte *)((int)&DAT_800ab0a6 + iVar4);
      iVar5 = iVar5 + -1;
      puVar3[2] = (ushort)*(byte *)((int)&DAT_800ab0a6 + iVar4 + 1);
      GetClut(0x100,0x1ea);
      *puVar3 = uVar2;
      puVar3 = puVar3 + 10;
    } while (iVar5 != 0);
    iVar4 = iVar4 + -4;
  } while (uVar6 != 0);
  DAT_800d5c38 = 0xa2;
  DAT_800d5c3a = 0x46;
  DAT_800d5bff = 3;
  return;
}


