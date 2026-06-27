uint FUN_80026dd4(void)

{
  uint uVar1;
  uint uVar2;
  
  printf("Wslot_check:\n");
  FUN_800c0158(0x140,0xc0,7,0);
  FUN_80029ac8(2);
  uVar1 = FUN_800275f0(0);
  uVar2 = FUN_800275f0(1);
  return uVar1 & 1 | (uVar2 & 1) << 1;
}
