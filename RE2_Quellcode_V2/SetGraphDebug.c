/* SetGraphDebug @ 0x80090178  (Ghidra headless, raw MIPS overlay) */

int SetGraphDebug(int level)

{
  uint uVar1;
  
  uVar1 = (uint)DAT_800b2702;
  DAT_800b2702 = (byte)level;
  if ((level & 0xffU) != 0) {
    (*(code *)PTR_printf_800b26fc)
              ("SetGraphDebug:level:%d,type:%d reverse:%d\n",level & 0xffU,DAT_800b2700,DAT_800b2703
              );
  }
  return uVar1;
}


