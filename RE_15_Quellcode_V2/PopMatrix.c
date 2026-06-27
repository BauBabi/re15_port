void PopMatrix(void)

{
  int iVar1;
  undefined4 unaff_retaddr;
  
  iVar1 = DAT_8007d680;
  if (DAT_8007d680 < 1) {
    DAT_8007d674 = unaff_retaddr;
    printf("");
    return;
  }
  DAT_8007d680 = DAT_8007d680 + -0x20;
  gte_ldR11R12(*(undefined4 *)((int)&DAT_8007d664 + iVar1));
  gte_ldR13R21(*(undefined4 *)(&DAT_8007d668 + iVar1));
  gte_ldR22R23(*(undefined4 *)(&DAT_8007d66c + iVar1));
  gte_ldR31R32(*(undefined4 *)(&DAT_8007d670 + iVar1));
  gte_ldR33(*(undefined4 *)((int)&DAT_8007d674 + iVar1));
  gte_ldtr(*(undefined4 *)(&DAT_8007d678 + iVar1),*(undefined4 *)(&DAT_8007d67c + iVar1),
           *(undefined4 *)((int)&DAT_8007d680 + iVar1));
  return;
}
