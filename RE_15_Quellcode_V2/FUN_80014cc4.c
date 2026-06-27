void FUN_80014cc4(void)

{
  *(undefined2 *)(&DAT_800bbe78 + (uint)DAT_800aca34 * 0x10) = DAT_800bbe68;
  *(undefined2 *)(&DAT_800bbe7c + (uint)DAT_800aca34 * 0x10) = DAT_800bbe6c;
  *(undefined2 *)(&DAT_800bbe7a + (uint)DAT_800aca34 * 0x10) = DAT_800bbe6a;
  *(undefined2 *)(&DAT_800bbe7e + (uint)DAT_800aca34 * 0x10) = DAT_800bbe6e;
  (&DAT_800bbe74)[(uint)DAT_800aca34 * 0x10] = DAT_800bbe66;
  (&DAT_800bbe75)[(uint)DAT_800aca34 * 0x10] = DAT_800bbe66;
  (&DAT_800bbe76)[(uint)DAT_800aca34 * 0x10] = 0;
  SetTile((TILE *)(&UNK_800bbe70 + (uint)DAT_800aca34 * 0x10));
  SetSemiTrans(&UNK_800bbe70 + (uint)DAT_800aca34 * 0x10,1);
  SetDrawMode((DR_MODE *)(&UNK_800bbe90 + (uint)DAT_800aca34 * 0xc),1,0,0x40,(RECT *)0x0);
  AddPrim(&UNK_800aa6a8 + (uint)DAT_800aca34 * 0x20,&UNK_800bbe70 + (uint)DAT_800aca34 * 0x10);
  AddPrim(&UNK_800aa6a8 + (uint)DAT_800aca34 * 0x20,&UNK_800bbe90 + (uint)DAT_800aca34 * 0xc);
  return;
}
