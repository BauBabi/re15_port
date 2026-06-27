/* FUN_80039b30 @ 0x80039b30  (Ghidra headless, raw MIPS overlay) */

void FUN_80039b30(undefined1 param_1,undefined2 param_2,undefined2 param_3,undefined1 param_4)

{
  short sVar1;
  int iVar2;
  
  iVar2 = DAT_800ea23c;
  *(undefined1 *)(DAT_800ea23c + 0x30f) = param_1;
  *(undefined1 *)(iVar2 + 0x30e) = param_4;
  *(short *)(iVar2 + 0x300) = DAT_800a2708 + 0x30;
  sVar1 = DAT_800a270a;
  *(undefined2 *)(iVar2 + 0x304) = param_2;
  *(undefined2 *)(iVar2 + 0x306) = param_3;
  *(undefined1 *)(iVar2 + 799) = param_1;
  *(undefined1 *)(iVar2 + 2) = 1;
  *(undefined1 *)(iVar2 + 3) = 0;
  *(undefined1 *)(iVar2 + 0x2de) = 0;
  *(short *)(iVar2 + 0x302) = sVar1 + 0x8e;
  return;
}


