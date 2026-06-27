/* GetClut @ 0x8008f828  (Ghidra headless, raw MIPS overlay) */

u_short GetClut(int x,int y)

{
  return (ushort)(y << 6) | (ushort)(x >> 4) & 0x3f;
}


