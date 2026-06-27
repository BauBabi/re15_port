/* DecDCToutSync @ 0x80094ab4  (Ghidra headless, raw MIPS overlay) */

int DecDCToutSync(int mode)

{
  int iVar1;
  uint uVar2;
  
  if (mode == 0) {
    MDEC_out_sync();
    iVar1 = LIBPRESS_OBJ_270();
    return iVar1;
  }
  uVar2 = MDEC_status();
  return uVar2 >> 0x18 & 1;
}


