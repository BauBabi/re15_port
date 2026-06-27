void FUN_80010000(void)

{
  byte *pbVar1;
  uint uVar2;
  
  pbVar1 = &DAT_800a73a4;
  if (DAT_800a73a4 != DAT_800aca34) {
    FUN_80010140();
    *pbVar1 = DAT_800aca34;
    FUN_8004f090();
    FUN_800280b4();
    FUN_8004f0b0();
    uVar2 = (uint)((int)DAT_800a7394 - (int)(&DAT_800c7934 + (uint)DAT_800aca34 * 0x2800)) / 0x14;
    if (DAT_80076d04 < uVar2) {
      DAT_80076d04 = uVar2;
    }
    uVar2 = (uint)((int)DAT_800a7398 - (int)(&DAT_80091394 + (uint)DAT_800aca34 * 0x800)) >> 4;
    if (DAT_80076d08 < uVar2) {
      DAT_80076d08 = uVar2;
    }
    DAT_800a7394 = &DAT_800c7934;
    DAT_800a7398 = &DAT_80091394;
    if (DAT_800aca34 == 0) {
      DAT_800a7394 = &DAT_800ca134;
      DAT_800a7398 = &DAT_80091b94;
    }
  }
  return;
}
