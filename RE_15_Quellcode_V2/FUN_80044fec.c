void FUN_80044fec(uint param_1)

{
  *(short *)(&UNK_80074828 +
            (param_1 >> 0xf & 0x1fe) + (uint)(byte)(&DAT_800748fc)[param_1 >> 0x18] * 2) =
       (short)param_1;
  return;
}
