/* FUN_8006a0cc @ 0x8006a0cc  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8006a0cc(int param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = (uint)DAT_800d5bf8;
  uVar3 = (uint)(byte)(&DAT_800d4a3d)[uVar4 * 4];
  iVar2 = FUN_80077360(&DAT_800d46cc,param_1);
  if (iVar2 != 0) {
    return 1;
  }
  if (uVar3 != 0xff) {
    if ((param_1 == 0xf) || (param_1 == 0x12)) {
      if ((short)(DAT_800d5c1c + 1) < 8) {
        DAT_800d5c1c = DAT_800d5c1c + 1;
        return 1;
      }
      DAT_800d5c1c = 0;
      bVar1 = uVar3 == 0;
      uVar3 = uVar3 - 1;
      if (bVar1) {
        DAT_800d5c1c = 0;
        return 0;
      }
      FUN_800694b8(uVar4,uVar3);
    }
    if (param_1 == 0x10) {
      if ((short)(DAT_800d5c1c + 1) < 8) {
        DAT_800d5c1c = DAT_800d5c1c + 1;
        return 1;
      }
      DAT_800d5c1c = 0;
      if (uVar3 == 0) {
        DAT_800d5c1c = 0;
        return 0;
      }
      uVar3 = uVar3 - 1;
      FUN_800694b8(uVar4,uVar3);
    }
    if (param_1 == 0xe) {
      if (param_2 == 0) {
        iVar2 = uVar3 - 1;
      }
      else {
        iVar2 = uVar3 - 2;
      }
    }
    else {
      if (uVar3 == 0) {
        return 0;
      }
      iVar2 = uVar3 - 1;
    }
    FUN_800694b8(uVar4,iVar2);
  }
  return 1;
}


