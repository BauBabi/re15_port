undefined4 FUN_80013c50(undefined4 param_1,int param_2)

{
  DAT_800bed9c = 0;
  DAT_800be570 = '\x01';
  DAT_800be580 = param_1;
  FUN_80013f80();
  if (param_2 == 0) {
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
