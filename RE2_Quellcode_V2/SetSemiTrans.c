/* SetSemiTrans @ 0x8008f9cc  (Ghidra headless, raw MIPS overlay) */

void SetSemiTrans(void *p,int abe)

{
  if (abe != 0) {
    PRIM_OBJ_200();
    return;
  }
  *(byte *)((int)p + 7) = *(byte *)((int)p + 7) & 0xfd;
  return;
}


