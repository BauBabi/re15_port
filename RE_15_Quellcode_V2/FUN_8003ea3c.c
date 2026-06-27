void FUN_8003ea3c(void)

{
  DAT_800b3f6c = (DAT_800b3f6c + ((uint)(DAT_800b3f6c << 1) >> 0x10) & 0xffff) +
                 (DAT_800b3f6c << 1 & 0xffff0000U);
  DAT_800b0fea = (short)DAT_800b3f6c;
  return;
}
