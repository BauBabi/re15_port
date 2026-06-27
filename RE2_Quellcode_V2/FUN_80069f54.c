/* FUN_80069f54 @ 0x80069f54  (Ghidra headless, raw MIPS overlay) */

bool FUN_80069f54(int param_1)

{
  bool bVar1;
  int iVar2;
  
  if ((param_1 == 0x12) && (iVar2 = FUN_80077360(&DAT_800d46cc,0x12), iVar2 != 0)) {
    DAT_800cfbd8 = DAT_800cfbd8 | 0x800000;
  }
  if (((param_1 == 0xf) && ((&DAT_800d4a3d)[(uint)DAT_800d5bf8 * 4] == -1)) ||
     ((param_1 == 0x11 && ((&DAT_800d4a3d)[(uint)DAT_800d5bf8 * 4] == -1)))) {
    bVar1 = true;
    DAT_800cfbd8 = DAT_800cfbd8 | 0x800000;
  }
  else {
    if ((DAT_800d5bf8 != 0x80) && (iVar2 = FUN_80077360(&DAT_800d46cc,param_1), iVar2 != 0)) {
      return true;
    }
    if (param_1 == 0xe) {
      bVar1 = false;
      if (DAT_800d5bf8 != 0x80) {
        bVar1 = 2 < (byte)(&DAT_800d4a3d)[(uint)DAT_800d5bf8 * 4];
      }
    }
    else {
      bVar1 = false;
      if (DAT_800d5bf8 != 0x80) {
        bVar1 = (&DAT_800d4a3d)[(uint)DAT_800d5bf8 * 4] != '\0';
      }
    }
  }
  return bVar1;
}


