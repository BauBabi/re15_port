/* FUN_80066afc @ 0x80066afc  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80066afc(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_80015614(param_1,(int)*(short *)(param_1 + 0x1c4),(int)*(short *)(param_1 + 0x1c6),
                       0x15e);
  uVar2 = 1;
  if (iVar1 != 0) {
    FUN_80015558(param_1,(int)*(short *)(param_1 + 0x1c4),(int)*(short *)(param_1 + 0x1c6),
                 (int)*(short *)(param_1 + 0x1d4));
    if ((**(uint **)(param_1 + 0x178) & 0x4000) != 0) {
      FUN_8005c040(0,((**(uint **)(param_1 + 0x178) & 0x2000) >> 0xd) * 3 + 4,param_1);
      DAT_800cfbf6 = DAT_800cfbf6 | 2;
    }
    FUN_8002959c(param_1,param_2,param_3,0x200);
    uVar2 = 0;
  }
  return uVar2;
}


