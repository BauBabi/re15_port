void FUN_80036718(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a == 2) {
        FUN_80045630(2,0);
        _DAT_800aca58 = 7;
      }
      goto LAB_8003681c;
    }
    if (DAT_800aca5a != 0) goto LAB_8003681c;
    DAT_800aca5a = 1;
    DAT_800acae8 = 7;
    DAT_800acae9 = 0;
    DAT_800acae3 = 7;
    DAT_800acae0 = 0;
    DAT_800acae7 = DAT_800acae7 | 1;
    FUN_80045024(0x4030001,&DAT_800aca88);
    DAT_800aca3c = DAT_800aca3c | 0xc0;
  }
  cVar1 = FUN_8001f314(DAT_800acad8,DAT_800acbc0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
LAB_8003681c:
  FUN_800369f8(0,1);
  return;
}
