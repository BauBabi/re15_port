/* SetShadeTex @ 0x8008f9f4  (Ghidra headless, raw MIPS overlay) */

void SetShadeTex(void *p,int tge)

{
  if (tge != 0) {
    PRIM_OBJ_228();
    return;
  }
  *(byte *)((int)p + 7) = *(byte *)((int)p + 7) & 0xfe;
  return;
}


