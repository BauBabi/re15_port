/* DecDCTinSync @ 0x80094a78  (Ghidra headless, raw MIPS overlay) */

int DecDCTinSync(int mode)

{
  int iVar1;
  uint uVar2;
  
  if (mode == 0) {
    MDEC_in_sync();
    iVar1 = LIBPRESS_OBJ_234();
    return iVar1;
  }
  uVar2 = MDEC_status();
  return uVar2 >> 0x1d & 1;
}


