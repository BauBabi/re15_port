/* _SsContExpression @ 0x8007ae54  (Ghidra headless, raw MIPS overlay) */

void _SsContExpression(short param_1,short param_2,uchar param_3)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = (&DAT_800ea250)[param_1] + param_2 * 0xb0;
  bVar1 = *(byte *)(iVar3 + 0x17);
  iVar4 = iVar3 + (uint)bVar1;
  _SsVmSetProgVol(*(undefined1 *)(iVar3 + 0x26),*(undefined1 *)(iVar4 + 0x37),param_3);
  FUN_8008482c((int)(short)(param_1 | param_2 << 8),*(undefined1 *)(iVar3 + 0x26),
               *(undefined1 *)(iVar4 + 0x37),*(undefined2 *)((uint)bVar1 * 2 + iVar3 + 0x60),
               *(undefined1 *)(iVar4 + 0x27));
  uVar2 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(iVar3 + 0x90) = uVar2;
  return;
}


