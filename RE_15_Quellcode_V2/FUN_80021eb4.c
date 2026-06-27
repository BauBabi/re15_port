void FUN_80021eb4(void)

{
  DAT_800b0fe8 = 0xffff;
  DAT_800b0fe6 = 0xffff;
  DAT_800b0fcc = 0;
  FUN_8004ee60(&DAT_800aca38,7);
  FUN_8004ee60(&DAT_800afbb4,0x98e);
  DAT_800bbe5c = 0;
  DAT_800bbe5d = 0;
  DAT_800bbe5e = 0;
  DAT_800bbe5f = 0;
  DAT_800bbe60 = 0;
  DAT_800bbe61 = 0;
  DAT_800bbe62 = 0;
  DAT_800bbe63 = 0;
  DAT_800bbe64 = 0;
  DAT_800aca38 = DAT_800aca38 | 0xc00000;
  return;
}
