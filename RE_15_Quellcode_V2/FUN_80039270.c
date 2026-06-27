void FUN_80039270(void)

{
  DAT_800b2584 = DAT_800ac77c;
  DAT_800bb4d4 = (uint)*(byte *)(DAT_800ac778 + 7) * 4 + DAT_800ac77c;
  DAT_800bb4d8 = (uint)*(byte *)(DAT_800ac778 + 7) * 0x20 + DAT_800bb4d4;
  DAT_800ac77c = (uint)*(byte *)(DAT_800ac778 + 7) * 0x20 + DAT_800bb4d8;
  return;
}
