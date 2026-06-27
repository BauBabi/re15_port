/* FUN_8003317c @ 0x8003317c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003317c(void)

{
  bool bVar1;
  
  DAT_800d7626 = DAT_800d7624;
  if (DAT_800dfaea == 0) {
    bVar1 = DAT_800ce5e0 != '\0';
  }
  else {
    bVar1 = DAT_800ce5e0 == '\0';
  }
  DAT_800d762e = 0;
  DAT_800d75da = (-(ushort)bVar1 & 0xf0) + DAT_800d7628;
  DecDCTin((u_long *)(&DAT_800d7670)[DAT_800d7681],(uint)DAT_800d4238);
  DecDCTout(DAT_800d7678,(uint)DAT_800d762c);
  DecDCTinSync(0);
  return;
}


