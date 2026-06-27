/* FUN_8002ab84 @ 0x8002ab84  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ab84(int param_1,int param_2)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  uVar2 = (uint)DAT_800dfd6a;
  DAT_800dfd6a = (byte)(uVar2 + DAT_8009db3c);
  sVar1 = *(short *)(DAT_800dfc10 + 10);
  uVar2 = uVar2 + DAT_8009db3c & 0xff;
  FUN_8002ae10(0,7);
  iVar3 = 0;
  do {
    uVar2 = uVar2 + DAT_8009db38 & 0xff;
    if (DAT_8009db34 == 0) {
      trap(0x1c00);
    }
    if ((DAT_8009db34 == -1) && ((char)(&DAT_8009d9f4)[uVar2] == -0x80000000)) {
      trap(0x1800);
    }
    iVar4 = iVar3 + 1;
    FUN_8002ac94(iVar3,-param_1,(int)(char)(&DAT_8009d9f4)[uVar2] / DAT_8009db34 + param_2,
                 (int)sVar1);
    iVar3 = iVar4;
  } while (iVar4 != 0xf0);
  FUN_8002addc();
  return;
}


