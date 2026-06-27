void FUN_80020610(void)

{
  if ((DAT_800aca3c & 8) == 0) {
    if ((DAT_800aca3c & 4) == 0) {
      if ((DAT_800aca3c & 2) != 0) {
        FUN_80020794(0,0);
      }
    }
    else {
      FUN_80020674(2);
    }
  }
  else {
    FUN_80020674(1);
  }
  return;
}
