/* FUN_800698b4 @ 0x800698b4  (Ghidra headless, raw MIPS overlay) */

void FUN_800698b4(int param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined *puVar4;
  
  puVar4 = &DAT_800cc1e8;
  if (param_1 == 1) {
    uVar2 = DAT_800d46ac - 2;
    iVar3 = uVar2 * 4 + 8;
    puVar4 = &DAT_800cc1e8 + uVar2 * 4;
    do {
      uVar2 = uVar2 - 1;
      (&DAT_800d4a38)[iVar3] = puVar4[0x8850];
      iVar1 = (uVar2 >> 1) + uVar2;
      (&DAT_800d4a39)[iVar3] = puVar4[0x8851];
      (&DAT_800d4a3a)[iVar3] = puVar4[0x8852];
      FUN_80069d88(iVar1,iVar1 + 3);
      iVar3 = iVar3 + -4;
      puVar4 = puVar4 + -4;
    } while (uVar2 != 0);
    if (((int)(uint)DAT_800d5bf8 < (int)(DAT_800d46ac - 2)) && (DAT_800d5bf8 != 0x80)) {
      DAT_800d5bf8 = DAT_800d5bf8 + 2;
      return;
    }
  }
  else {
    uVar2 = 0;
    iVar3 = 8;
    do {
      iVar1 = (uVar2 >> 1) + uVar2;
      puVar4[0x8854] = (&DAT_800d4a3c)[iVar3];
      puVar4[0x8855] = (&DAT_800d4a3d)[iVar3];
      puVar4[0x8856] = (&DAT_800d4a3e)[iVar3];
      uVar2 = uVar2 + 1;
      FUN_80069d88(iVar1 + 3);
      puVar4 = puVar4 + 4;
      iVar3 = iVar3 + 4;
    } while (uVar2 < DAT_800d46ac - 2);
    FUN_8006947c(DAT_800d46ac - 1,0,0,0);
    FUN_80069e54(0xb);
    FUN_8006947c(DAT_800d46ac - 2,0,0,0);
    FUN_80069e54(0xc);
    if ((1 < DAT_800d5bf8) && (DAT_800d5bf8 != 0x80)) {
      DAT_800d5bf8 = DAT_800d5bf8 - 2;
      return;
    }
  }
  DAT_800d5bfa = 0;
  DAT_800d5bf8 = 0x80;
  return;
}


