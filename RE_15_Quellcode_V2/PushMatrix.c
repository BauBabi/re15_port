void PushMatrix(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 unaff_retaddr;
  
  if (0x27f < DAT_8007d680) {
    DAT_8007d674 = unaff_retaddr;
    printf((char *)FUN_8007d904);
    return;
  }
  uVar1 = gte_stR11R12();
  uVar2 = gte_stR13R21();
  *(undefined4 *)(&DAT_8007d684 + DAT_8007d680) = uVar1;
  *(undefined4 *)(&DAT_8007d688 + DAT_8007d680) = uVar2;
  uVar1 = gte_stR22R23();
  uVar2 = gte_stR31R32();
  *(undefined4 *)(&DAT_8007d68c + DAT_8007d680) = uVar1;
  *(undefined4 *)(&DAT_8007d690 + DAT_8007d680) = uVar2;
  uVar1 = gte_stR33();
  *(undefined4 *)(&DAT_8007d694 + DAT_8007d680) = uVar1;
  uVar1 = gte_stTRX();
  uVar2 = gte_stTRY();
  uVar3 = gte_stTRZ();
  *(undefined4 *)(&DAT_8007d698 + DAT_8007d680) = uVar1;
  *(undefined4 *)(&DAT_8007d69c + DAT_8007d680) = uVar2;
  *(undefined4 *)(&DAT_8007d6a0 + DAT_8007d680) = uVar3;
  DAT_8007d680 = DAT_8007d680 + 0x20;
  return;
}
