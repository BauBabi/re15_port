/* _SsVmSetProgPan @ 0x80083fe4  (Ghidra headless, raw MIPS overlay) */

uint _SsVmSetProgPan(short param_1,short param_2,undefined1 param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = _SsVmVSetUp((int)param_1,(int)param_2);
  uVar2 = 0xffffffff;
  if (iVar1 == 0) {
    iVar1 = param_2 * 0x10;
    *(undefined1 *)(iVar1 + DAT_800d7838 + 4) = param_3;
    uVar2 = (uint)*(byte *)(iVar1 + DAT_800d7838 + 4);
  }
  return uVar2;
}


