void _SsSetPitchBend(short param_1,short param_2)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = (uint)param_1;
  iVar4 = (int)param_2;
  iVar3 = iVar4 * 0xac + (&DAT_800bb500)[uVar5];
  puVar2 = *(undefined1 **)(iVar3 + 4);
  *(undefined1 **)(iVar3 + 4) = puVar2 + 1;
  SpuVmPitchBend(iVar4 << 8 | uVar5,(int)*(short *)(iVar3 + 0x4c),
                 *(undefined1 *)((uint)*(byte *)(iVar3 + 0x12) + iVar3 + 0x2c),*puVar2);
  uVar1 = _SsReadDeltaValue(uVar5,iVar4);
  *(undefined4 *)(iVar3 + 0x88) = uVar1;
  return;
}
