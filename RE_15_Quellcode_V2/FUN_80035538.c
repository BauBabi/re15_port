void FUN_80035538(void)

{
  char cVar1;
  
  if (DAT_800aca5b != 1) {
    if (1 < DAT_800aca5b) {
      if (DAT_800aca5b == 2) {
        _DAT_800aca5a = 1;
      }
      goto LAB_80035780;
    }
    if (DAT_800aca5b != 0) goto LAB_80035780;
    _DAT_800aca5a = CONCAT11(1,DAT_800aca5a);
    DAT_800acae8 = 0xd;
    DAT_800acae9 = 0;
    DAT_800acae3 = 7;
    DAT_800acae0 = 0;
    DAT_800acaec = DAT_800acaec & 0x1fff | 0x4000;
    DAT_800acaf3 = FUN_8003703c(2000);
    DAT_800aca3c = DAT_800aca3c | 0xc0;
    FUN_80045024(0x1080001,&DAT_800aca88);
  }
  if ((DAT_800acaf3 & 1) != 0) {
    FUN_8001a8f8(DAT_800acbfc + 0x34,0xc0);
  }
  if ((DAT_800ac768 & 8) != 0) {
    DAT_800acabe = DAT_800acabe - (ushort)(byte)(&DAT_80074090)[(DAT_800aca5d - 1) * 5];
  }
  if ((DAT_800ac768 & 2) != 0) {
    DAT_800acabe = (ushort)(byte)(&DAT_80074090)[(DAT_800aca5d - 1) * 5] + DAT_800acabe;
  }
  cVar1 = FUN_8001f314(DAT_800acbc4,DAT_800acbc8,0,0x200);
  _DAT_800aca5a = CONCAT11(DAT_800aca5b + cVar1,DAT_800aca5a);
  if ((uRam00000000 & 0x10000) != 0) {
    uRam0000076c = DAT_800bb4b4;
    uRam00000774 = DAT_800bbe04;
    uRam00000770 = DAT_800b8988;
    uRam00000778 = DAT_800bbd90;
    DAT_800aca54 = DAT_800aca54 | 0x4000;
    *(undefined4 *)(*(int *)(DAT_800ac784 + 0x188) + 0xa14) = 0;
  }
LAB_80035780:
  FUN_800369f8(0,0);
  return;
}
