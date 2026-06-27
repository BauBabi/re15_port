void FUN_80048904(void)

{
  if (DAT_800aca38 < 0) {
    if (((DAT_800ac760 & 0x2000) == 0) || (-2 < (int)(uint)DAT_800b25be)) {
      if (((DAT_800ac760 & 0x8000) == 0) || (DAT_800b25be == 0)) {
        if ((DAT_800ac760 & 0x4000) == 0) {
          if (((DAT_800ac760 & 0x1000) != 0) && (1 < DAT_800b25be)) {
            DAT_800b25be = DAT_800b25be - 2;
          }
        }
        else if ((DAT_800b25be != 0xfffffffe) && (DAT_800b25be != 0xffffffff)) {
          DAT_800b25be = DAT_800b25be + 2;
        }
      }
      else if ((DAT_800b25be & 1) != 0) {
        DAT_800b25be = DAT_800b25be - 1;
      }
    }
    else if ((DAT_800b25be & 1) == 0) {
      DAT_800b25be = DAT_800b25be + 1;
    }
  }
  return;
}
