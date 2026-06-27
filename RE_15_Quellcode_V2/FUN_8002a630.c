undefined4 FUN_8002a630(undefined2 param_1)

{
  short sVar1;
  u_long uVar2;
  undefined2 uVar3;
  uint uVar4;
  undefined4 uVar5;
  u_long *local_18;
  u_long *local_14;
  
  uVar4 = 0;
  do {
    uVar2 = StGetNext(&local_18,&local_14);
    if (uVar2 == 0) {
      uVar5 = 1;
      if ((uRam0000001c < uRam00000024._2_2_) && (uRam00000004 <= uRam0000001c)) {
        uVar5 = 0;
      }
      sVar1 = (short)(0xf0 - uRam00000024._2_2_ >> 1);
      if (DAT_800aca34 != '\0') {
        sVar1 = sVar1 + 0xf0;
      }
      uRam00000000 = CONCAT22(sVar1,param_1) & 0xffff03ff;
      uVar3 = 0x10;
      if (uRam00000078 == 1) {
        uVar3 = 0x18;
      }
      uRam00000004 = CONCAT22(uRam00000024._2_2_,uVar3);
      uRam0000000c = 0;
      uRam00000010 = (uRam00000024 & 0xffff) >> 4;
      if (uRam00000078 == 1) {
        puRam00000008 = (u_long *)((uint)uRam00000024._2_2_ * 0xc);
      }
      else {
        puRam00000008 = (u_long *)((uint)uRam00000024._2_2_ << 3);
      }
      DecDCTReset(1);
      DecDCTin(*(u_long **)(uRam00000010 * 4),(uint)uRam00000078);
      DecDCTout(puRam00000008,(int)puRam00000008);
      uRam00000010 = uRam00000010 ^ 1;
      uRam00000014 = *local_14;
      uRam00000018 = local_14[1];
      uRam0000001c = local_14[2];
      uRam00000020 = local_14[3];
      uRam00000028 = local_14[5];
      uRam0000002c = local_14[6];
      uRam00000030 = local_14[7];
      uRam00000024._0_2_ = (short)local_14[4];
      uRam00000024._2_2_ = (ushort)(local_14[4] >> 0x10);
      uRam00000024 = CONCAT22(uRam00000024._2_2_ + 8,(short)uRam00000024 + 8) & 0xfff0fff0;
      DecDCTinSync(0);
      DecDCToutSync(0);
      DecDCTvlc(local_18,*(u_long **)(uRam00000010 * 4));
      StFreeRing(local_18);
      return uVar5;
    }
    FUN_80061fc0(0);
    uVar4 = uVar4 + 1;
  } while (uVar4 < 0xb4);
  return 1;
}
