void FUN_80029a98(int param_1,undefined4 param_2)

{
  *(undefined4 *)(&DAT_800b2928 + param_1 * 0x80) = param_2;
  (&DAT_800b2924)[param_1 * 0x40] = 2;
  return;
}
