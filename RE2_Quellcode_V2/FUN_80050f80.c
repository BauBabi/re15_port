/* FUN_80050f80 @ 0x80050f80  (Ghidra headless, raw MIPS overlay) */

int FUN_80050f80(int param_1)

{
  return (uint)(*(ushort *)(param_1 + 10) >> 0xb) * -100 +
         (*(ushort *)(param_1 + 10) >> 6 & 0x1f) * -0x708;
}


