/* FUN_80074014 @ 0x80074014  (Ghidra headless, raw MIPS overlay) */

void FUN_80074014(int param_1)

{
  byte bVar1;
  u_short uVar2;
  undefined1 *puVar3;
  int iVar4;
  u_short *puVar5;
  int iVar6;
  int iVar7;
  undefined *puVar8;
  
  puVar8 = &DAT_80198c80;
  uVar2 = GetClut(0,0x1e4);
  iVar4 = 6;
  if (param_1 != 0) {
    iVar4 = 0x11;
  }
  iVar4 = iVar4 * 4;
  do {
    iVar6 = iVar4 + -4;
    iVar7 = 2;
    puVar5 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar5 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar5 + -7) = 100;
      *(undefined1 *)(puVar5 + -5) = 0x80;
      *(undefined1 *)((int)puVar5 + -9) = 0x80;
      *(undefined1 *)(puVar5 + -4) = 0x80;
      *(undefined1 *)(puVar5 + -1) = *(undefined1 *)((int)&DAT_800aafdc + iVar4);
      *(undefined1 *)((int)puVar5 + -1) = *(undefined1 *)((int)&DAT_800aafdc + iVar4 + 1);
      puVar8 = puVar8 + 0x14;
      puVar5[1] = (ushort)*(byte *)((int)&DAT_800aafde + iVar4);
      bVar1 = *(byte *)((int)&DAT_800aafde + iVar4 + 1);
      iVar7 = iVar7 + -1;
      *puVar5 = uVar2;
      puVar5[2] = (ushort)bVar1;
      puVar5 = puVar5 + 10;
    } while (iVar7 != 0);
    iVar4 = iVar6;
  } while (iVar6 != 0);
  iVar4 = 2;
  puVar3 = &DAT_80198c06;
  do {
    iVar4 = iVar4 + -1;
    puVar3[-3] = 5;
    puVar3[1] = 0x2a;
    puVar3[-2] = 0x10;
    puVar3[-1] = 0x40;
    *puVar3 = 0x10;
    puVar3 = puVar3 + 0x18;
  } while (iVar4 != 0);
  DAT_800d5c3c = 7;
  DAT_800d5c3e = 0x4e;
  return;
}


