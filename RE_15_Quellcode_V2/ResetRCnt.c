undefined4 ResetRCnt(uint param_1)

{
  undefined4 uVar1;
  
  if ((param_1 & 0xffff) < 3) {
    *(undefined2 *)(&TMR_DOTCLOCK_VAL + (param_1 & 0xffff) * 4) = 0;
    uVar1 = COUNTER_OBJ_170();
    return uVar1;
  }
  return 0;
}
