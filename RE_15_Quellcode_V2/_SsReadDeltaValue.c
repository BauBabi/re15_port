void _SsReadDeltaValue(int param_1,short param_2)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((param_1 << 0x10) >> 0xe));
  pbVar3 = *(byte **)(iVar5 + 4);
  *(byte **)(iVar5 + 4) = pbVar3 + 1;
  bVar1 = *pbVar3;
  uVar4 = (uint)bVar1;
  if (uVar4 == 0) {
    SEQREAD_OBJ_1D50();
    return;
  }
  iVar2 = uVar4 << 2;
  if ((bVar1 & 0x80) != 0) {
    uVar4 = uVar4 & 0x7f;
    do {
      pbVar3 = *(byte **)(iVar5 + 4);
      *(byte **)(iVar5 + 4) = pbVar3 + 1;
      bVar1 = *pbVar3;
      uVar4 = uVar4 * 0x80 + (bVar1 & 0x7f);
    } while ((bVar1 & 0x80) != 0);
    iVar2 = uVar4 * 4;
  }
  *(uint *)(iVar5 + 0x80) = (iVar2 + uVar4) * 2 + *(int *)(iVar5 + 0x80);
  return;
}
