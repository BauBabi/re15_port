/* FUN_8005b9f4 @ 0x8005b9f4  (Ghidra headless, raw MIPS overlay) */

void FUN_8005b9f4(uint param_1)

{
  *(short *)(&DAT_800d46d0 +
            (param_1 >> 0xf & 0x1fe) + (uint)(byte)(&DAT_800a80af)[param_1 >> 0x18] * 2) =
       (short)param_1;
  return;
}


