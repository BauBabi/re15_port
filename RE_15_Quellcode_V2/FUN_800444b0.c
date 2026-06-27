void FUN_800444b0(void)

{
  if ((DAT_800b52ad != -1) && (DAT_800b52ad == '\0')) {
    SsSeqReplay((short)DAT_800b52ae);
    DAT_800b52ac = 1;
  }
  if (DAT_800b52b5 != -1) {
    if (DAT_800b52b5 == '\0') {
      SsSeqReplay((short)DAT_800b52b6);
      DAT_800b52b4 = 1;
    }
    if (DAT_800b52bd == '\0') {
      SsSeqReplay((short)DAT_800b52be);
      DAT_800b52bc = 1;
    }
  }
  return;
}
