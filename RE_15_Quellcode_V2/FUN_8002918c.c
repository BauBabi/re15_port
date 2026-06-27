void FUN_8002918c(void)

{
  short *psVar1;
  short *psVar2;
  
  psVar1 = DAT_800b851c;
  psVar2 = &DAT_800b841c;
  if (DAT_800b851c != &DAT_800b841c) {
    do {
      if ((psVar2[2] & 0x80U) == 0) {
        psVar2 = (short *)FUN_80029214(psVar2 + 2,(int)*psVar2 + psVar2[1] * 0x10000);
      }
      else {
        psVar2 = (short *)FUN_8002939c();
      }
    } while (psVar2 < psVar1);
  }
  return;
}
