short SsUtPitchBend(short param_1,short param_2,short param_3,short param_4,short param_5)

{
  short sVar1;
  undefined2 in_register_0000001a;
  undefined2 in_register_0000001e;
  
  SpuVmVSetUp((int)param_2,(int)param_3,CONCAT22(in_register_0000001a,param_3),
              CONCAT22(in_register_0000001e,param_4));
  DAT_800b532a = 0x21;
  sVar1 = SpuVmPBVoice((int)param_1,0x21,(int)param_2,(int)param_3,param_5);
  return -(ushort)(sVar1 == 0);
}
