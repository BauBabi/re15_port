/* FUN_80069210 @ 0x80069210  (Ghidra headless, raw MIPS overlay) */

void FUN_80069210(int param_1)

{
  int iVar1;
  int iVar2;
  
  FUN_80012fb8(*(undefined2 *)(&DAT_800a3fe4 + (uint)*(byte *)(param_1 + 8) * 2),&DAT_80198000,1,
               &DAT_80011b28);
  iVar1 = DAT_80198000;
  *(undefined1 *)(param_1 + 0x105) = 1;
  *(undefined1 *)(param_1 + 0x104) = 0x17;
  iVar2 = *(int *)(&DAT_80198008 + iVar1);
  *(undefined **)(param_1 + 0x14) = &UNK_801cd420 + iVar2;
  *(undefined **)(param_1 + 0x1c) = &UNK_801cd43c + iVar2;
  DAT_800cfbf0._1_1_ = 1;
  DAT_800cfbf0._0_1_ = 0x17;
  FUN_80076a40((int)&DAT_80198000 + *(int *)(&DAT_8019800c + iVar1));
  DrawSync(0);
  return;
}


