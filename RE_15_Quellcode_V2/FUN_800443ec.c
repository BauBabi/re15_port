void FUN_800443ec(void)

{
  SsUtKeyOffV(0x10);
  SsUtKeyOffV(0x11);
  SsUtKeyOffV(0x12);
  SsUtKeyOffV(0x13);
  SsUtKeyOffV(0x14);
  SsUtKeyOffV(0x15);
  SsUtKeyOffV(0x16);
  SsUtKeyOffV(0x17);
  DAT_800b5570 = &UNK_80074828 + _DAT_800b0fe2 * 2 + (uint)(byte)(&DAT_800748fc)[DAT_800b0fe0] * 2;
  if ((*DAT_800b5570 & 0x3f) != (DAT_800b2b44 & 0x3f)) {
    FUN_800449f4(0x3c);
  }
  return;
}
