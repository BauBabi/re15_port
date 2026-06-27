void SpuVmNoiseOn(undefined2 param_1,undefined2 param_2)

{
  byte bVar1;
  
  DAT_800b5323 = 0x7f;
  bVar1 = SpuVmAlloc(0xff);
  DAT_800b532e = (ushort)bVar1;
  if (DAT_800b532e < DAT_800b52a8) {
    vmNoiseOn2(bVar1,param_1,param_2,0x80ff,0x5fc8);
  }
  return;
}
