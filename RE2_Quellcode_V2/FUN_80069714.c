/* FUN_80069714 @ 0x80069714  (Ghidra headless, raw MIPS overlay) */

void FUN_80069714(void)

{
  int iVar1;
  undefined *puVar2;
  uint uVar3;
  uint uVar4;
  
  iVar1 = FUN_80069668(0);
  uVar3 = iVar1 + 1;
  if (iVar1 == 9) {
    FUN_80069e54(0xd);
    DAT_800d4a60 = 0;
    DAT_800d4a61 = 0;
    DAT_800d4a62 = 0;
  }
  else if ((-1 < iVar1) && (uVar3 < 10)) {
    puVar2 = &DAT_800cc1e8 + uVar3 * 4;
    iVar1 = uVar3 * 4 + -4;
    do {
      uVar4 = uVar3;
      FUN_80069d88((uVar4 >> 1) + uVar4,((uVar4 - 1 >> 1) - 1) + uVar4);
      (&DAT_800d4a3c)[iVar1] = puVar2[0x8854];
      (&DAT_800d4a3d)[iVar1] = puVar2[0x8855];
      (&DAT_800d4a3e)[iVar1] = puVar2[0x8856];
      if (DAT_800d5bf8 == uVar4) {
        DAT_800d5bf8 = DAT_800d5bf8 - 1;
      }
      if (DAT_800d69f4 == uVar4) {
        DAT_800d69f4 = DAT_800d69f4 - 1;
      }
      puVar2 = puVar2 + 4;
      uVar3 = uVar4 + 1;
      iVar1 = iVar1 + 4;
    } while (uVar3 < DAT_800d46ac);
    FUN_80069e54(((uVar4 >> 1) - 1) + uVar3);
    iVar1 = uVar4 * 4;
    (&DAT_800d4a3c)[iVar1] = 0;
    (&DAT_800d4a3d)[iVar1] = 0;
    (&DAT_800d4a3e)[iVar1] = 0;
  }
  return;
}


