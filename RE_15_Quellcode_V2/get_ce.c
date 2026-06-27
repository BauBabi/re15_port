uint get_ce(short param_1,short param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = (uint)param_1;
  uVar1 = 0;
  if ((-1 < (int)uVar3) && (uVar1 = uVar3, (int)((int)DAT_8007e354 - 1U) < (int)uVar3)) {
    uVar1 = (int)DAT_8007e354 - 1U;
  }
  uVar2 = (uint)param_2;
  uVar3 = 0;
  if ((-1 < (int)uVar2) && (uVar3 = uVar2, (int)((int)DAT_8007e356 - 1U) < (int)uVar2)) {
    uVar3 = (int)DAT_8007e356 - 1U;
  }
  if (DAT_8007e350 - 1 < 2) {
    return (uVar3 & 0xfff) << 0xc | uVar1 & 0xfff | 0xe4000000;
  }
  uVar1 = SYS_OBJ_1774();
  return uVar1;
}
