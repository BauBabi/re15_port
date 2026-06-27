/* SsSetReservedVoice @ 0x80084e90  (Ghidra headless, raw MIPS overlay) */

char SsSetReservedVoice(char param_1)

{
  char cVar1;
  
  if (((byte)param_1 < 0x19) && (param_1 != '\0')) {
    DAT_800d7854 = param_1;
    return param_1;
  }
  cVar1 = VS_SRV_OBJ_28();
  return cVar1;
}


