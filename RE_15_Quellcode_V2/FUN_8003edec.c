void FUN_8003edec(undefined1 *param_1,int param_2)

{
  param_1[1] = 1;
  *param_1 = 0;
  param_1[4] = 0xff;
  param_1[8] = 0xff;
  *(undefined1 **)(param_1 + 0x140) = param_1 + (char)param_1[2] * 0x20 + 0xc0;
  *(uint *)(param_1 + 0x1c) = DAT_800b3f70 + (uint)*(ushort *)(param_2 * 2 + DAT_800b3f70);
  return;
}
