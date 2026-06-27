undefined4 SetRCnt(uint param_1,undefined2 param_2,uint param_3)

{
  undefined4 uVar1;
  ushort uVar2;
  
  param_1 = param_1 & 0xffff;
  uVar2 = 0x48;
  if (2 < param_1) {
    uVar1 = COUNTER_OBJ_98();
    return uVar1;
  }
  *(undefined2 *)(&TMR_DOTCLOCK_MODE + param_1 * 4) = 0;
  *(undefined2 *)(&TMR_DOTCLOCK_MAX + param_1 * 4) = param_2;
  if (param_1 < 2) {
    uVar2 = 0x48;
    if ((param_3 & 0x10) != 0) {
      uVar2 = 0x49;
    }
    if ((param_3 & 1) == 0) {
      uVar1 = COUNTER_OBJ_74();
      return uVar1;
    }
  }
  else if ((param_1 == 2) && ((param_3 & 1) == 0)) {
    uVar2 = 0x248;
  }
  if ((param_3 & 0x1000) != 0) {
    uVar2 = uVar2 | 0x10;
  }
  *(ushort *)(&TMR_DOTCLOCK_MODE + param_1 * 4) = uVar2;
  return 1;
}
