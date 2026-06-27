void FUN_80030444(void)

{
  u_long uVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  
  DAT_800ac75c = _DAT_800ac758;
  _DAT_800ac758 = PadRead(0);
  DAT_800ac770 = DAT_800ac768;
  uVar1 = _DAT_800ac758;
  DAT_800ac768 = 0;
  uVar4 = 0xf;
  do {
    uVar3 = uVar4 & 0xff;
    if ((*(ushort *)((&PTR_DAT_80073e1c)[DAT_800b0fcc] + uVar3 * 2) & _DAT_800ac758) != 0) {
      DAT_800ac768 = 1 << (uVar4 & 0x1f) | DAT_800ac768;
    }
    uVar4 = uVar4 - 1;
  } while (uVar3 != 0);
  if ((DAT_800aca40 & 0x1000000) != 0) {
    DAT_800ac768 = DAT_800ac768 & 0xf000;
  }
  DAT_800ac75c = (DAT_800ac75c ^ _DAT_800ac758) & _DAT_800ac758;
  uVar4 = DAT_800ac75c;
  DAT_800ac760 = DAT_800ac758;
  DAT_800ac76c = (DAT_800ac770 ^ DAT_800ac768) & DAT_800ac768;
  DAT_800ac764 = (short)(_DAT_800ac758 >> 0x10);
  DAT_800ac766 = (short)(DAT_800ac75c >> 0x10);
  DAT_800ac762 = (undefined2)DAT_800ac75c;
  cVar2 = DAT_800b5454;
  if (((DAT_800ac75c & DAT_800b5450) == 0) && (cVar2 = DAT_800b5455, DAT_80073e2c != '\0')) {
    if ((_DAT_800ac758 & DAT_800b5450) != 0) {
      DAT_80073e2c = DAT_80073e2c + -1;
    }
    DAT_800aca38 = DAT_800aca38 & 0x7fffffff;
  }
  else {
    DAT_800aca38 = DAT_800aca38 | 0x80000000;
    DAT_80073e2c = cVar2;
  }
  _DAT_800ac758 = uVar1;
  DAT_800ac75c = uVar4;
  return;
}
