/* FUN_8002aa6c @ 0x8002aa6c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002aa6c(int param_1)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  
  uVar4 = DAT_800dfd6b + 1 & 0xf;
  iVar5 = uVar4 * 2;
  DAT_800dfd6b = (byte)uVar4;
  if (param_1 == 1) {
    bVar1 = (&DAT_8009daf4)[iVar5];
    uVar2 = (&DAT_8009daf5)[iVar5];
  }
  else {
    bVar1 = (&DAT_8009db14)[iVar5];
    uVar2 = (&DAT_8009db15)[iVar5];
  }
  if ((DAT_800cfbd8 & 2) == 0) {
    iVar5 = 0;
    sVar3 = *(short *)(DAT_800dfc10 + 10);
    FUN_8002ae10(0,7);
    do {
      FUN_8002ac94(iVar5,-(uint)bVar1,uVar2,(int)sVar3);
      iVar5 = iVar5 + 1;
    } while (iVar5 != 0xf0);
    FUN_8002addc();
  }
  else {
    FUN_8002ab84((uint)bVar1,uVar2);
  }
  return;
}


