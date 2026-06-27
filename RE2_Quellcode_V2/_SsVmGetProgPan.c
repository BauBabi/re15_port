/* _SsVmGetProgPan @ 0x80084058  (Ghidra headless, raw MIPS overlay) */

uint _SsVmGetProgPan(short param_1,short param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = _SsVmVSetUp((int)param_1,(int)param_2);
  uVar2 = 0xffffffff;
  if (iVar1 == 0) {
    uVar2 = (uint)*(byte *)(param_2 * 0x10 + DAT_800d7838 + 4);
  }
  return uVar2;
}


