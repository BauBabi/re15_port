void FUN_8002137c(void)

{
  DrawSync(0);
  FUN_80061fc0(DAT_800b5456);
  FUN_800458d4();
  PutDrawEnv((DRAWENV *)(&DAT_800b5370 + (uint)DAT_800aca34 * 0x5c));
  PutDispEnv((DISPENV *)(&DAT_800b5428 + (uint)DAT_800aca34 * 0x14));
  if (((DAT_800aca3c & 0x80) == 0) && (DAT_800b0fe4 != DAT_800afbb5)) {
    DAT_800b5457 = '\x01';
  }
  if (DAT_800b536c == '\0') {
    if ((DAT_800b5457 != '\0') && ((DAT_800aca38 & 0x200000) == 0)) {
      FUN_80021bbc();
      goto LAB_800215fc;
    }
    FUN_80043870(-(DAT_800aca34 != 0) & 0xf0);
  }
  (&DAT_800b5439)[(uint)DAT_800aca34 * 0x14] = 0;
  DrawOTag((u_long *)(&UNK_800ac714 + (uint)DAT_800aca34 * 0x40));
  DrawOTag((u_long *)(&UNK_800ab6d4 + (uint)DAT_800aca34 * 0x1000));
  DrawOTag((u_long *)(&DAT_800aa6b4 + (uint)DAT_800aca34 * 0x20));
  DAT_800aca34 = DAT_800aca34 ^ 1;
LAB_800215fc:
  if ((DAT_800aca38 & 0x200000) == 0) {
    DAT_800b5457 = '\0';
  }
  return;
}
