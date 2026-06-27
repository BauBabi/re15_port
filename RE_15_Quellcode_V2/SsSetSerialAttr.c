void SsSetSerialAttr(char param_1,char param_2,char param_3)

{
  SpuCommonAttr local_30;
  
  if (param_1 == '\0') {
    if (param_2 == '\0') {
      local_30.mask = 0x200;
      local_30.cd.mix = (long)(byte)param_3;
    }
    if (param_2 == '\x01') {
      local_30.mask = 0x100;
      local_30.cd.reverb = (long)(byte)param_3;
    }
  }
  if (param_1 == '\x01') {
    if (param_2 == '\0') {
      local_30.mask = 0x2000;
      local_30.ext.mix = (long)(byte)param_3;
    }
    if (param_2 == '\x01') {
      local_30.mask = 0x1000;
      local_30.ext.reverb = (long)(byte)param_3;
    }
  }
  SpuSetCommonAttr(&local_30);
  return;
}
