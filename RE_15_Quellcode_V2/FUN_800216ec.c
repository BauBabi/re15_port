void FUN_800216ec(int param_1,ushort param_2,undefined4 param_3,undefined4 *param_4)

{
  if ((&DAT_800b545a)[param_1 * 0x22] == 0) {
    (&DAT_800b5458)[param_1 * 0x22] = param_2;
    if (param_4 != (undefined4 *)0x0) {
      *(undefined4 *)(&DAT_800b546c + param_1 * 0x22) = *param_4;
      *(undefined4 *)(&DAT_800b547c + param_1 * 0x22) = *param_4;
      *(undefined4 *)(&DAT_800b5470 + param_1 * 0x22) = param_4[1];
      *(undefined4 *)(&DAT_800b5480 + param_1 * 0x22) = param_4[1];
    }
  }
  else {
    (&DAT_800b5458)[param_1 * 0x22] = -(ushort)((short)(&DAT_800b545a)[param_1 * 0x22] < 1) & 0x7fff
    ;
  }
  return;
}
