void FUN_8001f1e0(undefined4 *param_1)

{
  short sVar1;
  
  sVar1 = *(short *)((int)param_1 + 0x96) + *(short *)((int)param_1 + 0x9a);
  *(short *)((int)param_1 + 0x96) = sVar1;
  param_1[0x16] = (int)sVar1 + param_1[0x16];
  if (0x1c < *(ushort *)(param_1 + 0x27)) {
    *param_1 = 0;
  }
  return;
}
