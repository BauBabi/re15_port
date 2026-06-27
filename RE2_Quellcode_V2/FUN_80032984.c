/* FUN_80032984 @ 0x80032984  (Ghidra headless, raw MIPS overlay) */

void FUN_80032984(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint *puVar1;
  uint uVar2;
  undefined4 uStack_c;
  
  uVar2 = (uint)&stack0xfffffff3 & 3;
  puVar1 = (uint *)(&stack0xfffffff3 + -uVar2);
  *puVar1 = *puVar1 & -1 << (uVar2 + 1) * 8 | DAT_80010af0 >> (3 - uVar2) * 8;
  uVar2 = (int)&uStack_c + 3U & 3;
  puVar1 = (uint *)(((int)&uStack_c + 3U) - uVar2);
  *puVar1 = *puVar1 & -1 << (uVar2 + 1) * 8 | DAT_80010af4 >> (3 - uVar2) * 8;
  uStack_c = DAT_80010af4;
  uVar2 = DAT_80010af0;
  FUN_80033118();
  if ((DAT_800d7632 & 1) != 0) {
    FUN_8002bda8(0,0,param_3,param_4,uVar2);
  }
  if ((DAT_800d7632 & 0x40) != 0) {
    FUN_8002bda8(2,0,param_3,param_4,uVar2);
  }
  if ((DAT_800d7632 & 0x80) != 0) {
    FUN_8002bda8(2,0xffffff,param_3,param_4,uVar2);
  }
  if ((DAT_800d7632 & 8) == 0) {
    DAT_800d4238 = 0;
    DAT_800d7682 = 0x140;
    DAT_800cfb74 = DAT_800cfb74 | 0x8000;
  }
  else if (DAT_800d7634 == 0) {
    FUN_8002bda8(0,0);
    DAT_800dfc1b = 2;
  }
  else {
    DAT_800d4238 = 0;
    DAT_800d7682 = 0x140;
  }
  return;
}


