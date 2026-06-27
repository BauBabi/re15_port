void SEQREAD_OBJ_D84(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  undefined4 uVar1;
  int unaff_s0;
  short unaff_s1;
  short unaff_s2;
  
  if (*(char *)(unaff_s0 + 0x16) == '(') {
    if (*(code **)(&UNK_800bb580 + unaff_s2 * 4 + unaff_s1 * 0x40) != (code *)0x0) {
      (**(code **)(&UNK_800bb580 + unaff_s2 * 4 + unaff_s1 * 0x40))
                ((int)unaff_s1,(int)unaff_s2,param_3);
    }
  }
  uVar1 = _SsReadDeltaValue((int)unaff_s1,(int)unaff_s2);
  *(undefined4 *)(unaff_s0 + 0x88) = uVar1;
  return;
}
