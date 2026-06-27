/* FUN_80081530 @ 0x80081530  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80081530(ushort param_1,short *param_2,short *param_3)

{
  undefined4 uVar1;
  
  if (param_1 < 0x18) {
    SpuGetVoiceVolume((int)(short)param_1,param_2,param_3);
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffffffff;
  }
  return uVar1;
}


