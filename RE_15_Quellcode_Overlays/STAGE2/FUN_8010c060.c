/* FUN_8010c060 @ 0x8010c060  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c060(void)

{
  char cVar1;
  undefined2 uVar2;
  short sVar3;
  short *unaff_s0;
  
  uVar2 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar2;
  if ((ushort)((ushort)_DAT_800ac784[0x76] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x76] & 1;
  }
  uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee7418))();
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee73d8))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(0x9c4);
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x1e2) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e2) = *(char *)((int)_DAT_800ac784 + 0x1e2) + -1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x1e3) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e3) = *(char *)((int)_DAT_800ac784 + 0x1e3) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    sVar3 = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
    *(short *)((int)_DAT_800ac784 + 0x1de) = sVar3;
    DAT_800aca58 = (undefined1)sVar3;
    cVar1 = func_0x8001a780(unaff_s0 + -0x4d);
    DAT_800aca59 = cVar1 + '\x02';
    DAT_800aca5a = 0;
    if (*unaff_s0 < 0) {
      DAT_800aca58 = 3;
      DAT_800aca59 = '\0';
    }
    DAT_800acae7 = DAT_800acae7 | 1;
    if (DAT_800acae7 != 0) {
      func_0x80019700(0x2000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0xa54,
                      0x80118bb8);
      FUN_8010d390();
      return;
    }
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}


