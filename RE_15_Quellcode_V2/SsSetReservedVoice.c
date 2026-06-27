char SsSetReservedVoice(char param_1)

{
  char cVar1;
  
  if (((byte)param_1 < 0x19) && (param_1 != '\0')) {
    DAT_800b52a8 = param_1;
    return param_1;
  }
  cVar1 = VS_SRV_OBJ_30();
  return cVar1;
}
