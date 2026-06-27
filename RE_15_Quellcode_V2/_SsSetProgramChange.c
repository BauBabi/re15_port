void _SsSetProgramChange(short param_1,short param_2,undefined1 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = param_2 * 0xac + (&DAT_800bb500)[param_1];
  *(undefined1 *)(iVar2 + (uint)*(byte *)(iVar2 + 0x12) + 0x2c) = param_3;
  uVar1 = _SsReadDeltaValue();
  *(undefined4 *)(iVar2 + 0x88) = uVar1;
  return;
}
