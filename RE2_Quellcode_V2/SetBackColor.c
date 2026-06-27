/* SetBackColor @ 0x8008dde4  (Ghidra headless, raw MIPS overlay) */

void SetBackColor(long rbk,long gbk,long bbk)

{
  gte_ldbkdir(rbk << 4,gbk << 4,bbk << 4);
  return;
}


