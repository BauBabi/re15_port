undefined4 GetRCnt(ushort param_1)

{
  undefined4 uVar1;
  
  if (param_1 < 3) {
    uVar1 = COUNTER_OBJ_D0();
    return uVar1;
  }
  return 0;
}
