/* FUN_80028ea4 @ 0x80028ea4  (Ghidra headless, raw MIPS overlay) */

void FUN_80028ea4(uint *param_1)

{
  undefined2 uVar1;
  
  if ((*(ushort *)((int)param_1 + 0x9e) & 0x7fff) != 0) {
    *(ushort *)((int)param_1 + 0x9e) = *(ushort *)((int)param_1 + 0x9e) - 1;
    *(short *)((int)param_1 + 0x9a) =
         *(short *)((int)param_1 + 0x9a) + (short)*(char *)((int)param_1 + 0x9d);
    *(short *)(param_1 + 0x26) = (short)param_1[0x26] + (short)(char)param_1[0x27];
    if (*(short *)((int)param_1 + 0x9e) == -0x8000) {
      *(undefined2 *)(param_1 + 0xe) = 0;
      *(undefined2 *)((int)param_1 + 0x86) = 0;
      *(undefined2 *)(param_1 + 0xf) = 0;
      *(undefined2 *)((int)param_1 + 0x3a) = 0;
      *(undefined1 *)((int)param_1 + 0x79) = 0x1e;
      *(undefined2 *)((int)param_1 + 0x3e) = 0;
      *param_1 = *param_1 | 0x1062;
      uVar1 = *(undefined2 *)(DAT_800ce330 + 0x76);
      *(undefined2 *)((int)param_1 + 0x42) = 0x400;
      *(undefined2 *)(param_1 + 0x10) = uVar1;
    }
  }
  return;
}


