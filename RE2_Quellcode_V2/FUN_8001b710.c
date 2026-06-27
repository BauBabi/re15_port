/* FUN_8001b710 @ 0x8001b710  (Ghidra headless, raw MIPS overlay) */

void FUN_8001b710(uint param_1)

{
  int iVar1;
  int iVar2;
  undefined *puVar3;
  uint uVar4;
  
  puVar3 = &DAT_800cc1e8;
  if (*(int *)(&DAT_800cffdc + param_1 * 4) == 0) {
    return;
  }
  uVar4 = param_1;
  if (param_1 - 0x10 < 0x10) {
    uVar4 = 0x10;
  }
  iVar2 = 0;
  if (uVar4 == 0x10) {
    iVar2 = 1;
    puVar3 = &UNK_800cc1ec;
    do {
      if (*(short *)(puVar3 + 0x4134) == 0x10) {
        *(undefined2 *)(puVar3 + 0x4136) = 1;
        *(undefined4 *)(&DAT_800cfe5c + param_1 * 4) =
             *(undefined4 *)(&DAT_800cffdc + iVar2 * 0x180 + param_1 * 4);
        return;
      }
      iVar2 = iVar2 + -1;
      puVar3 = puVar3 + -4;
    } while (-1 < iVar2);
    if (DAT_800d0322 == 0) {
LAB_8001b82c:
      iVar2 = 1;
    }
    else {
      iVar1 = iVar2 * 4;
      if (DAT_800d031e != 0) goto LAB_8001b834;
      iVar2 = 0;
    }
  }
  else {
    do {
      iVar2 = iVar2 + 1;
      if (*(ushort *)(puVar3 + 0x4134) == uVar4) {
        *(undefined2 *)(puVar3 + 0x4136) = 1;
        return;
      }
      puVar3 = puVar3 + 4;
    } while (iVar2 < 2);
    if (DAT_800d031e != 0) {
      iVar1 = iVar2 * 4;
      if (DAT_800d0322 != 0) goto LAB_8001b834;
      goto LAB_8001b82c;
    }
    iVar2 = 0;
  }
  iVar1 = iVar2 << 2;
LAB_8001b834:
  *(undefined2 *)((int)&DAT_800d031e + iVar1) = 1;
  *(short *)((int)&DAT_800d031c + iVar1) = (short)uVar4;
  *(undefined4 *)(&DAT_800cfe5c + param_1 * 4) =
       *(undefined4 *)(&DAT_800cffdc + iVar2 * 0x180 + param_1 * 4);
  iVar1 = ((uVar4 - 0x10) * 4 + 1) - iVar2;
  DAT_800d531c = 0x1d5;
  DAT_800d5308 = *(undefined4 *)(&DAT_8009adf8 + iVar1 * 8);
  DAT_80099753 = (&DAT_8009b754)[iVar1];
  DAT_800d531e = (&DAT_8009b754)[iVar1];
  DAT_800d5314 = *(int *)(&DAT_8009adf4 + iVar1 * 8) +
                 (uint)DAT_80099752 * 0x10000 + (uint)DAT_80099750;
  FUN_80012fb8(0x1d5,&DAT_80100000 + iVar2 * 0xd000,3,&DAT_800108c4);
  return;
}


