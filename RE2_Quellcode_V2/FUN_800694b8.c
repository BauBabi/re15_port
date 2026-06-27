/* FUN_800694b8 @ 0x800694b8  (Ghidra headless, raw MIPS overlay) */

void FUN_800694b8(int param_1,undefined1 param_2)

{
  int iVar1;
  char cVar2;
  
  iVar1 = param_1 * 4;
  cVar2 = (&DAT_800d4a3e)[iVar1];
  if (cVar2 == '\x01') {
    (&DAT_800d4a3d)[(param_1 + 1) * 4] = param_2;
    cVar2 = (&DAT_800d4a3e)[iVar1];
  }
  if (cVar2 == '\x02') {
    (&DAT_800d4a3d)[(param_1 + -1) * 4] = param_2;
  }
  (&DAT_800d4a3d)[iVar1] = param_2;
  return;
}


