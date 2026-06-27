/* SetIntrMask @ 0x80086200  (Ghidra headless, raw MIPS overlay) */

undefined2 SetIntrMask(undefined2 param_1)

{
  undefined2 uVar1;
  
  uVar1 = *(undefined2 *)PTR_I_MASK_800accd4;
  *(undefined2 *)PTR_I_MASK_800accd4 = param_1;
  return uVar1;
}


