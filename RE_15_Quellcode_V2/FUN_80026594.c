void FUN_80026594(int param_1)

{
  int iVar1;
  
  FUN_800217b0(0x200,0x1800,7,0);
  FUN_800216ec(0,0,0,0);
  while (iVar1 = FUN_8002178c(0), iVar1 == 0) {
    FUN_80029ac8(1);
  }
  FUN_800217b0(0x200,0,7,0);
  FUN_800216ec(0,0x7fff,0xffffff,0);
  FUN_80029ac8(1);
  DAT_800b5456 = 2;
  if (param_1 == 0) {
    DAT_800b5457 = 2;
    FUN_80043850(0x140,0xf0,0);
  }
  return;
}
