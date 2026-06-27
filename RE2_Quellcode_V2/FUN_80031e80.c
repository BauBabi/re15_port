/* FUN_80031e80 @ 0x80031e80  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80031e80(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_80012fb8(*(undefined4 *)(&DAT_800a20b8 + param_2 * 0xc),
                       *(undefined4 *)(&PTR_PTR_800a20c0)[param_2 * 3],0,"execute");
  FUN_80031f6c(param_1,(&PTR_DAT_800a20bc)[param_2 * 3]);
  return uVar1;
}


