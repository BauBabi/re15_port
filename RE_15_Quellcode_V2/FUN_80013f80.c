void FUN_80013f80(void)

{
  if ((DAT_800be574 & 0x7ff) != 0) {
    DAT_800be578 = (DAT_800be574 & 0xfffff800) + DAT_800be580;
    FUN_800104b0(&DAT_800be584,DAT_800be578,0x800);
  }
  return;
}
