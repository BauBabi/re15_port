uint FUN_800130c4(int param_1,undefined4 *param_2)

{
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = *(undefined4 *)(&DAT_8006f43c + param_1 * 8);
  }
  return (uint)*(uint3 *)(&DAT_8006f440 + param_1 * 8);
}
