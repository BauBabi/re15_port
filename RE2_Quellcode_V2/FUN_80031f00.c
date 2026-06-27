/* FUN_80031f00 @ 0x80031f00  (Ghidra headless, raw MIPS overlay) */

void FUN_80031f00(int param_1)

{
  FUN_80012fb8(*(undefined4 *)(&DAT_800a20b8 + param_1 * 0xc),
               *(undefined4 *)(&PTR_PTR_800a20c0)[param_1 * 3],0,"chain");
  FUN_8003208c((&PTR_DAT_800a20bc)[param_1 * 3]);
  return;
}


