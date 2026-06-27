/* FUN_8003a8cc @ 0x8003a8cc  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a8cc(undefined4 *param_1,undefined4 *param_2)

{
  *param_1 = *param_2;
  param_1[1] = param_2[1];
  param_1[2] = param_2[2] + (*(ushort *)((int)param_2 + 10) + 0x7b40) * 0x10000;
  param_1[3] = (uint)*(byte *)(param_2 + 3) + (uint)*(byte *)((int)param_2 + 0xd) * 0x100 +
               (uint)*(byte *)((int)param_2 + 0xe) * 0x10000 +
               (uint)*(byte *)((int)param_2 + 0xf) * 0x1000000;
  return;
}


