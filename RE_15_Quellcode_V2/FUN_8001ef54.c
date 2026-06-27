void FUN_8001ef54(uint *param_1)

{
  uint uVar1;
  
  if ((*param_1 & 0x40) == 0) {
    FUN_80022da0(param_1[0x1b],param_1 + 6,param_1 + 0x10);
  }
  if ((*param_1 & 0x20) != 0) {
    FUN_8001f024(param_1,param_1 + 0x10);
  }
  uVar1 = *param_1;
  if ((uVar1 & 1) != 0) {
    if ((uVar1 & 0x18) == 0) {
      if ((uVar1 & 0x80) != 0) {
        FUN_8002441c(param_1 + 1,param_1[0x25],param_1[0x26]);
      }
    }
    else if ((uVar1 & 8) == 0) {
      if ((uVar1 & 0x10) != 0) {
        FUN_8001f220(param_1);
      }
    }
    else {
      FUN_8001f1e0(param_1);
    }
  }
  return;
}
