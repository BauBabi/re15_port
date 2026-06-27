/* FUN_800530ec @ 0x800530ec  (Ghidra headless, raw MIPS overlay) */

void FUN_800530ec(undefined1 *param_1,int param_2)

{
  ushort uVar1;
  int iVar2;
  
  iVar2 = DAT_800d8cbc;
  uVar1 = *(ushort *)(param_2 * 2 + DAT_800d8cbc);
  *(undefined1 **)(param_1 + 0x140) = param_1 + (char)param_1[2] * 0x20 + 0xc0;
  param_1[1] = 1;
  *param_1 = 0;
  param_1[4] = 0xff;
  param_1[8] = 0xff;
  *(uint *)(param_1 + 0x1c) = iVar2 + (uint)uVar1;
  return;
}


