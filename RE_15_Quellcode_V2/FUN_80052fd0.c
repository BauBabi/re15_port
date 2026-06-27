void FUN_80052fd0(void)

{
  uint uVar1;
  
  if (DAT_800a73a4 != DAT_800aca34) {
    FUN_8004f090();
    FUN_80053240();
    FUN_8004f0b0();
    uVar1 = (uint)((int)DAT_800a739c - (int)(&DAT_80092394 + (uint)DAT_800aca34 * 0x800)) >> 4;
    if (DAT_80076d0c < uVar1) {
      DAT_80076d0c = uVar1;
    }
    uVar1 = (uint)((int)DAT_800a73a0 - (int)(&DAT_80093394 + (uint)DAT_800aca34 * 0xa000)) / 0x28;
    if (DAT_80076d10 < uVar1) {
      DAT_80076d10 = uVar1;
    }
  }
  DAT_800a739c = &DAT_80092394;
  if (DAT_800aca34 == 0) {
    DAT_800a739c = &DAT_80092b94;
  }
  DAT_800a73a0 = &DAT_80093394;
  if (DAT_800aca34 == 0) {
    DAT_800a73a0 = &DAT_8009d394;
  }
  return;
}
