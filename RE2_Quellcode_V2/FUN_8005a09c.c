/* FUN_8005a09c @ 0x8005a09c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005a09c(void)

{
  short sVar1;
  int iVar2;
  ulong uVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = 0;
  DAT_800e2a84 = 1;
  while( true ) {
    FUN_80031f94(1);
    SpuSetTransferMode(0);
    uVar4 = (uint)DAT_800d424b;
    uVar3 = DAT_800eae40 + 0x14440;
    if (DAT_800d4c4b != -1) {
      SsVabClose((short)DAT_800d4c4b);
      DAT_800d4c4b = -1;
    }
    if (uVar4 == 0xff) break;
    iVar2 = uVar4 * 0x10;
    DAT_800dbb84 = &DAT_801f8e10;
    DAT_800d5308 = *(undefined4 *)(&DAT_800a7b1c + iVar2);
    DAT_800d531c = 0x161;
    DAT_800d5314 = (uint)DAT_800993b0 + (uint)DAT_800993b2 * 0x10000 +
                   (uint)*(ushort *)(&DAT_800a7b20 + iVar2) +
                   (uint)(byte)(&DAT_800a7b22)[iVar2] * 0x10000;
    DAT_800d531e = (&DAT_800a7b23)[iVar2];
    iVar2 = FUN_80012fb8(0x161,&DAT_801f8e10,3,"              ENEM EDT");
    DAT_800d75ac = &DAT_801f8e10 + *(int *)(&DAT_801f8e08 + iVar2);
    do {
      FUN_80031f94(1);
      sVar1 = SsVabTransCompleted(0);
    } while (sVar1 == 0);
    sVar1 = SsVabOpenHeadSticky(DAT_800d75ac,3,uVar3);
    DAT_800d4c4b = (char)sVar1;
    if (DAT_800d4c4b == -1) {
      iVar5 = iVar5 + 1;
      FUN_800594c8(3,0);
      if (iVar5 == 8) {
        FUN_80031f94(1);
        return;
      }
    }
    else {
      iVar5 = 0;
      uVar4 = uVar4 << 4 | 8;
      DAT_800d5308 = *(undefined4 *)(&DAT_800a7b1c + uVar4);
      DAT_800d531c = 0x161;
      DAT_800d5314 = (uint)DAT_800993b0 + (uint)DAT_800993b2 * 0x10000 +
                     (uint)*(ushort *)(&DAT_800a7b20 + uVar4) +
                     (uint)(byte)(&DAT_800a7b22)[uVar4] * 0x10000;
      DAT_800d531e = (&DAT_800a7b23)[uVar4];
      iVar2 = FUN_800132b0(0x161,1,&DAT_801bfa14,(int)DAT_800d4c4b,"              ENEM VBD");
      if (iVar2 != -1) {
        FUN_80029c94();
        FUN_80031f94(1);
        DAT_800e2a84 = 0;
        return;
      }
    }
  }
  return;
}


