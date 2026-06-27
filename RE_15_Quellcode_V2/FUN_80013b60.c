undefined4 FUN_80013b60(int param_1,undefined4 param_2,int param_3)

{
  DAT_800be572 = (undefined2)param_1;
  param_1 = param_1 * 8;
  DAT_800bed9c = 0;
  DAT_800be570 = '\x01';
  DAT_800be574 = *(undefined4 *)(&DAT_8006f43c + param_1);
  DAT_800be57c = (uint)(byte)(&DAT_8006f442)[param_1] * 0x10000 +
                 (uint)*(ushort *)(&DAT_8006f440 + param_1);
  DAT_800be580 = param_2;
  FUN_80013f80();
  if (param_3 == 0) {
    FUN_80013df0(1);
  }
  else {
    FUN_80013df0(0);
    do {
      FUN_80029ac8(1);
      FUN_80013df0(0);
    } while (DAT_800be570 != '\0');
  }
  FUN_80013fdc();
  return DAT_800be574;
}
