/* FUN_80038950 @ 0x80038950  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80038950(int *param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  
  iVar1 = (uint)*(ushort *)(param_2 + 0x90) + param_3;
  param_3 = (uint)*(ushort *)(param_2 + 0x92) + param_3;
  if (((uint)((*param_1 - *(int *)(param_2 + 0x84)) + iVar1) <= (uint)(iVar1 * 2)) &&
     ((uint)((param_1[2] - *(int *)(param_2 + 0x8c)) + param_3) <= (uint)(param_3 * 2))) {
    if (param_4 == 0) {
      return 1;
    }
    iVar1 = *(ushort *)(param_2 + 0x9e) - 1;
    if ((-1 < iVar1) && ((uint)((param_1[1] - *(int *)(param_2 + 0x88)) + iVar1) <= iVar1 * 2 + 1U))
    {
      return 1;
    }
  }
  return 0;
}


