/* FUN_8002c2b0 @ 0x8002c2b0  (Ghidra headless, raw MIPS overlay) */

void FUN_8002c2b0(int param_1,undefined2 param_2,uint param_3,undefined4 *param_4)

{
  byte bVar1;
  undefined4 uVar2;
  
  bVar1 = (&DAT_800dfc2f)[param_1 * 0x4c];
  (&DAT_800dfc1c)[param_1 * 0x26] = param_2;
  *(uint *)(&DAT_800dfc1c + param_1 * 0x26 + (uint)DAT_800ce5e0 * 8 + 8) =
       param_3 | (uint)bVar1 << 0x18;
  if (param_4 == (undefined4 *)0x0) {
    uVar2 = 0xf00140;
    (&DAT_800dfc60)[param_1 * 0x13] = 0;
  }
  else {
    (&DAT_800dfc60)[param_1 * 0x13] = *param_4;
    uVar2 = param_4[1];
  }
  (&DAT_800dfc64)[param_1 * 0x13] = uVar2;
  return;
}


