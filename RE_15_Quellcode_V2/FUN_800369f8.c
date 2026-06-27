void FUN_800369f8(int param_1,int param_2)

{
  int iVar1;
  long lVar2;
  int iVar3;
  undefined1 auStack_38 [20];
  uint local_24;
  uint local_1c;
  
  iVar3 = *(int *)(DAT_800ac784 + 0x188);
  RotMatrix((SVECTOR *)(DAT_800ac784 + 0x68),(MATRIX *)(DAT_800ac784 + 0x20));
  FUN_80022da0(DAT_800ac784 + 0x20,iVar3 + 0x18,auStack_38);
  FUN_80022da0(auStack_38,iVar3 + 0xc4,auStack_38);
  iVar3 = iVar3 + param_2 * 0x204 + 0x2b0;
  FUN_80022da0(auStack_38,iVar3 + -0x140,auStack_38);
  FUN_80022da0(auStack_38,iVar3 + -0x94,auStack_38);
  FUN_80022da0(auStack_38,iVar3 + 0x18,auStack_38);
  if (param_1 == 0) {
    *(uint *)(DAT_800ac784 + 0x34) =
         *(int *)(DAT_800ac784 + 0x34) - (local_24 - *(int *)(iVar3 + 0x54));
    *(uint *)(DAT_800ac784 + 0x3c) =
         *(int *)(DAT_800ac784 + 0x3c) - (local_1c - *(int *)(iVar3 + 0x5c));
  }
  else {
    iVar1 = (int)(((local_24 & 0xffff) - (uint)*(ushort *)(iVar3 + 0x54)) * 0x10000) >> 0x10;
    iVar3 = (int)(((local_1c & 0xffff) - (uint)*(ushort *)(iVar3 + 0x5c)) * 0x10000) >> 0x10;
    lVar2 = SquareRoot0(iVar1 * iVar1 + iVar3 * iVar3);
    *(short *)(DAT_800ac784 + 0x8c) = (short)lVar2;
  }
  return;
}
