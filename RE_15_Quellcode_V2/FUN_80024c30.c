void FUN_80024c30(int param_1)

{
  uint uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  undefined1 auStack_88 [96];
  
  iVar4 = *(int *)(param_1 + 0x188);
  RotMatrix((SVECTOR *)(param_1 + 0x68),(MATRIX *)(param_1 + 0x20));
  FUN_80022da0((MATRIX *)(param_1 + 0x20),iVar4 + 0x18,auStack_88);
  FUN_80022da0(auStack_88,iVar4 + 0x578,iVar4 + 0x5a0);
  sVar2 = *(short *)(param_1 + 0x6a) + *(short *)(iVar4 + 0x62) + *(short *)(iVar4 + 0x5c2);
  uVar1 = *(ushort *)(iVar4 + 100) & 0xfff;
  *(short *)(iVar4 + 100) = (short)uVar1;
  if (uVar1 - 0x5dd < 999) {
    sVar2 = sVar2 + 0x800;
  }
  if ((DAT_800acc0c & 1) == 0) {
    sVar2 = sVar2 + *(short *)(iVar4 + 0x5f4);
  }
  iVar3 = (int)sVar2;
  FUN_80024e40(iVar4 + 0xb6c,iVar4 + 0x5a0,400,100,0x28,2,0,400,100,0x18,4,0,iVar3);
  FUN_80024e40(iVar4 + 0xc18,iVar4 + 0xbac,0x15e,0,0x30,4,300,0x96,0,0x18,6,0,iVar3);
  FUN_80024e40(iVar4 + 0xcc4,iVar4 + 0xc58,300,0,0x30,4,0,0x96,0,0x18,6,0,iVar3);
  FUN_80024e40(iVar4 + 0xd70,iVar4 + 0xd04,0xfa,0,0x30,4,0,100,0,0x18,6,0,iVar3);
  return;
}
