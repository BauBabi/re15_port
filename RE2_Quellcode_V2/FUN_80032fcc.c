/* FUN_80032fcc @ 0x80032fcc  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80032fcc(void)

{
  undefined2 *puVar1;
  u_long uVar2;
  uint uVar3;
  undefined4 uVar4;
  u_long *local_20;
  u_long *local_1c;
  
  uVar3 = 0;
  do {
    uVar2 = StGetNext(&local_20,&local_1c);
    puVar1 = DAT_800d763c;
    if (uVar2 == 0) {
      uVar3 = local_1c[2];
      *DAT_800d763c = (short)uVar3;
      uVar3 = uVar3 & 0xffff;
      uVar4 = 1;
      if ((uVar3 < DAT_800d767c) && ((ushort)puVar1[1] <= uVar3)) {
        uVar4 = 0;
      }
      DAT_800d763c[1] = *DAT_800d763c;
      FUN_8003317c();
      DAT_800d7681 = DAT_800d7681 ^ 1;
      DAT_800d7624 = -(ushort)((short)local_1c[4] != 0x140) & 0x30;
      DAT_800d7630 = (undefined2)((ushort)local_1c[4] + 8 >> 4);
      DecDCTvlc(local_20,(u_long *)(&DAT_800d7670)[DAT_800d7681]);
      StFreeRing(local_20);
      return uVar4;
    }
    VSync(0);
    uVar3 = uVar3 + 1;
  } while (uVar3 < 0xb4);
  DAT_800d7685 = 1;
  return 2;
}


