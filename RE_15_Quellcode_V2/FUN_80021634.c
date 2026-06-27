void FUN_80021634(int param_1,undefined4 param_2)

{
  DAT_800b536c = (char)param_1;
  DAT_800b53e4 = param_1 == 2;
  if (DAT_800b53e4) {
    DAT_800b538b = (undefined1)((uint)param_2 >> 0x10);
    DAT_800b5389 = (undefined1)param_2;
    DAT_800b538a = (undefined1)((uint)param_2 >> 8);
    DAT_800b53e5 = DAT_800b5389;
    DAT_800b53e6 = DAT_800b538a;
    DAT_800b53e7 = DAT_800b538b;
  }
  DAT_800b5388 = DAT_800b53e4;
  return;
}
