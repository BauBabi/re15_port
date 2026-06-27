/* FUN_8002e4f8 @ 0x8002e4f8  (Ghidra headless, raw MIPS overlay) */

int FUN_8002e4f8(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x1a4);
  DAT_800cc1e0 = param_2;
  DAT_800df334 = param_2 + 0x3000;
  DAT_800eae38 = *(undefined4 *)(iVar1 + 0x778);
  DAT_800d5220 = *(undefined4 *)(iVar1 + 0x76c);
  DAT_800ea248 = *(undefined4 *)(iVar1 + 0x770);
  DAT_800d5be0 = *(undefined4 *)(iVar1 + 0x774);
  return param_2 + 0x5200;
}


