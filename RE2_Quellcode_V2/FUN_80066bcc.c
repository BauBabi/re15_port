/* FUN_80066bcc @ 0x80066bcc  (Ghidra headless, raw MIPS overlay) */

bool FUN_80066bcc(int param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  long lVar4;
  
  FUN_80015558(param_1,(int)*(short *)(param_1 + 0x1c4),(int)*(short *)(param_1 + 0x1c6),
               ((uint)(*(ushort *)(param_1 + 0x144) >> 0xf) * -2 + 1) *
               (int)*(short *)(param_1 + 0x1d4));
  FUN_800152c8(param_1,0);
  if ((**(uint **)(param_1 + 0x178) & 0x4000) != 0) {
    uVar1 = **(uint **)(param_1 + 0x178) & 0x2000;
    if (*(char *)(param_1 + 5) == '\x05') {
      FUN_8005c040(1,(uVar1 >> 0xd) * 3 + 4,param_1);
      DAT_800cfbf6 = DAT_800cfbf6 | 4;
    }
    else {
      FUN_8005c040(0,(uVar1 >> 0xd) * 3 + 4,param_1);
      DAT_800cfbf6 = DAT_800cfbf6 | 2;
    }
  }
  FUN_8002959c(param_1,param_2,param_3,0x200);
  iVar2 = *(int *)(param_1 + 0x38) - (int)*(short *)(param_1 + 0x1c4);
  iVar3 = *(int *)(param_1 + 0x40) - (int)*(short *)(param_1 + 0x1c6);
  lVar4 = SquareRoot0(iVar2 * iVar2 + iVar3 * iVar3);
  return lVar4 < 100;
}


