void FUN_800487b0(void)

{
  if (DAT_800aca38 < 0) {
    if (((DAT_800ac760 & 0x2000) == 0) || (-2 < (int)(uint)DAT_800b25bd)) {
      if (((DAT_800ac760 & 0x8000) == 0) || (DAT_800b25bd == 0)) {
        if ((DAT_800ac760 & 0x4000) == 0) {
          if (((DAT_800ac760 & 0x1000) != 0) && (1 < DAT_800b25bd)) {
            DAT_800b25bd = DAT_800b25bd - 2;
          }
        }
        else if ((DAT_800b25bd != 0xfffffffe) && (DAT_800b25bd != 0xffffffff)) {
          DAT_800b25bd = DAT_800b25bd + 2;
        }
      }
      else if ((DAT_800b25bd & 1) != 0) {
        DAT_800b25bd = DAT_800b25bd - 1;
      }
    }
    else if ((DAT_800b25bd & 1) == 0) {
      DAT_800b25bd = DAT_800b25bd + 1;
    }
  }
  DAT_800b25be = DAT_800b25bd;
  return;
}
