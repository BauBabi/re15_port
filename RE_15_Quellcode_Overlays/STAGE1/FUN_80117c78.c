/* FUN_80117c78 @ 0x80117c78  (Ghidra headless overlay RE) */

void FUN_80117c78(void)

{
  char cVar1;
  undefined2 in_v0;
  int in_v1;
  short *unaff_s0;
  
  *(undefined2 *)(in_v1 + 0x1dc) = in_v0;
  DAT_800aca58 = 2;
  cVar1 = func_0x8001a780(unaff_s0 + -0x4d);
  DAT_800aca59 = cVar1 + '\x02';
  DAT_800aca5a = 0;
  if (*unaff_s0 < 0) {
    DAT_800aca58 = 3;
    DAT_800aca59 = '\0';
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  FUN_80118524();
  return;
}


