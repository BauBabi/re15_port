void trapIntr(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  
  if (DAT_800776f8 == 0) {
    printf("unexpected interrupt(%04x)\n",(uint)I_STAT);
    ReturnFromException();
  }
  DAT_800776fa = 1;
  uVar3 = (uint)(I_MASK & DAT_80077728 & I_STAT);
  if (uVar3 != 0) {
    do {
      puVar5 = &DAT_800776fc;
      for (uVar4 = 0; (uVar3 != 0 && ((int)uVar4 < 0xb)); uVar4 = uVar4 + 1) {
        if ((uVar3 & 1) != 0) {
          I_STAT = ~(ushort)(1 << (uVar4 & 0x1f));
          if ((code *)*puVar5 != (code *)0x0) {
            (*(code *)*puVar5)();
          }
        }
        puVar5 = puVar5 + 1;
        uVar3 = uVar3 >> 1;
      }
      uVar3 = (uint)(I_MASK & DAT_80077728 & I_STAT);
    } while (uVar3 != 0);
  }
  if ((I_STAT & I_MASK) == 0) {
    DAT_80078790 = 0;
  }
  else {
    iVar2 = DAT_80078790 + 1;
    bVar1 = 0x800 < DAT_80078790;
    DAT_80078790 = iVar2;
    if (bVar1) {
      printf("intr timeout(%04x:%04x)\n",(uint)I_STAT,(uint)I_MASK);
      DAT_80078790 = 0;
      I_STAT = 0;
      INTR_OBJ_428();
      return;
    }
  }
  DAT_800776fa = 0;
  ReturnFromException();
  return;
}
