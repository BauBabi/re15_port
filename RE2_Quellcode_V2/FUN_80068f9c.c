/* FUN_80068f9c @ 0x80068f9c  (Ghidra headless, raw MIPS overlay) */

void FUN_80068f9c(uint param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined *puVar4;
  
  uVar3 = 0;
  puVar4 = &DAT_800cc1e8;
  if (param_1 - 0xe < 2) {
    DAT_800ce5e2 = DAT_800d5bf8;
    DAT_800ce5e3 = DAT_800d46ac;
    FUN_8007730c(&DAT_800d4854,0x7f);
    iVar2 = 0xb;
    if (param_1 == 0xe) {
      DAT_800d5bfa = 3;
      DAT_800d5bf8 = 0;
      iVar2 = 0;
    }
    else {
      DAT_800d5bfa = 0;
      DAT_800d5bf8 = 0x80;
    }
    DAT_800d46ac = 8;
    puVar4 = &DAT_800cc1e8;
    do {
      puVar4[0x23fc] = puVar4[0x8854];
      puVar4[0x23fd] = puVar4[0x8855];
      iVar1 = (iVar2 + uVar3) * 3;
      puVar4[0x23fe] = puVar4[0x8856];
      puVar4[0x8854] = (&DAT_800a93d0)[iVar1];
      puVar4[0x8855] = (&DAT_800a93d1)[iVar1];
      uVar3 = uVar3 + 1;
      puVar4[0x8856] = (&DAT_800a93d2)[iVar1];
      puVar4 = puVar4 + 4;
    } while (uVar3 < 0xb);
  }
  else {
    iVar2 = FUN_80077360(&DAT_800d4854,0x7f);
    if (iVar2 != 0) {
      do {
        uVar3 = uVar3 + 1;
        puVar4[0x8854] = puVar4[0x23fc];
        puVar4[0x8855] = puVar4[0x23fd];
        puVar4[0x8856] = puVar4[0x23fe];
        puVar4 = puVar4 + 4;
      } while (uVar3 < 10);
      DAT_800d46ac = DAT_800ce5e3;
      DAT_800d5bf8 = DAT_800ce5e2;
      if (DAT_800ce5e2 == 0x80) {
        DAT_800d5bfa = 0;
      }
      else {
        DAT_800d5bfa = (&DAT_800d4a3c)[(uint)DAT_800ce5e2 * 4];
      }
      DAT_800d4a64 = 0x30;
      if ((param_1 & 1) == 0) {
        DAT_800d4a64 = 0x2f;
      }
      DAT_800d4a65 = 1;
      DAT_800d4a66 = 0;
    }
    FUN_80077334(&DAT_800d4854,0x7f);
  }
  return;
}


