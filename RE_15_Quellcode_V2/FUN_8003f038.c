void FUN_8003f038(void)

{
  if ((DAT_800aca40 & 0x2000000) == 0) {
    FUN_8003ea3c();
    DAT_800b3f7a = 0;
    DAT_800b3f70 = *(undefined4 *)(DAT_800ac778 + 0x44);
    FUN_8003ee3c(1,1);
    FUN_8003f0a0();
  }
  return;
}
