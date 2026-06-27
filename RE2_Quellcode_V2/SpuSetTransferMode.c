/* SpuSetTransferMode @ 0x80079728  (Ghidra headless, raw MIPS overlay) */

long SpuSetTransferMode(long mode)

{
  long lVar1;
  
  if ((mode != 0) && (mode == 1)) {
    lVar1 = S_STM_OBJ_1C();
    return lVar1;
  }
  DAT_800ab288 = mode;
  DAT_800ab238 = 0;
  return 0;
}


