void FUN_800280b4(void)

{
  if ((DAT_800b8520 & 0x80) != 0) {
    FUN_80028134();
  }
  FUN_8002918c();
  if (DAT_80073bd8 < DAT_800b851c + 0x7ff47be4U) {
    DAT_80073bd8 = DAT_800b851c + 0x7ff47be4U;
  }
  FUN_80029560();
  return;
}
