void FUN_8002b898(uint param_1,int param_2)

{
  int iVar1;
  
  param_1 = param_1 & 0xff;
  iVar1 = param_1 * 0x94;
  if (DAT_800ac9a4 != param_2) {
    DAT_800ac9a4 = param_2;
    FUN_80022150(2,param_2,(&DAT_800b4018)[iVar1],(&DAT_800b4019)[iVar1]);
  }
  (&DAT_800b3fa8)[param_1 * 0x25] = param_2 + 0xc;
  (&DAT_800b3f9c)[iVar1] = 0;
  (&DAT_800b3f9d)[iVar1] = 0;
  (&DAT_800b3fb0)[param_1 * 0x25] = param_2 + 0x28;
  *(undefined4 *)(&DAT_800b4008 + iVar1) = 0x808080;
  DAT_800ac77c = FUN_80025940(&DAT_800b3fa4 + param_1 * 0x25,DAT_800ac77c);
  DAT_800ac77c = FUN_80025a98(&DAT_800b3fa4 + param_1 * 0x25);
  return;
}
