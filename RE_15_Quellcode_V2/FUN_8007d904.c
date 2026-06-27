void FUN_8007d904(void)

{
  if (DAT_800aca5d == '\f') {
    DAT_800aca5d = '\x13';
  }
  if (DAT_800aca5d == '\r') {
    DAT_800aca5d = '\b';
  }
  return;
}
