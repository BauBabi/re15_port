void FUN_80013fdc(void)

{
  uint uVar1;
  
  uVar1 = DAT_800be574 & 0x7ff;
  if (uVar1 != 0) {
    FUN_8004ee38(uVar1 + DAT_800be578,&DAT_800be584 + uVar1,0x800 - uVar1);
  }
  return;
}
