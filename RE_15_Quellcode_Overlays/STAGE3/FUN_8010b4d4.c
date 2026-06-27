/* FUN_8010b4d4 @ 0x8010b4d4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b4d4(void)

{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  uint *puVar6;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    puVar6 = _DAT_800ac784;
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x16;
        *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
        *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
        *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x474;
        *(ushort *)((int)_DAT_800ac784 + 0x6a) = ((byte)_DAT_800ac784[0x24] & 0xf0) << 4;
      }
      else if (bVar1 != 3) goto LAB_8010bf24;
      if (*(char *)((int)_DAT_800ac784 + 0x95) == 'F') {
        *(undefined2 *)(_DAT_800ac784 + 0x2c) = 0x474;
        *(short *)((int)_DAT_800ac784 + 0x1ba) = *(short *)((int)_DAT_800ac784 + 0x1ba) + -0x708;
      }
      iVar4 = func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
      if (iVar4 == 0) {
        return;
      }
      *(char *)((int)_DAT_800ac784 + 0x82) = *(char *)((int)_DAT_800ac784 + 0x82) + '\x01';
      _DAT_800ac784[0xe] = _DAT_800ac784[0xe] - 0x708;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x2c) = 0;
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) goto LAB_8010bf24;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  sVar2 = func_0x8001aa68(((byte)_DAT_800ac784[0x24] & 0xf0) << 4,0x10);
  puVar6 = _DAT_800ac784;
  *(short *)((int)_DAT_800ac784 + 0x6a) = sVar2 + *(short *)((int)_DAT_800ac784 + 0x6a);
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
LAB_8010bf24:
  uVar3 = func_0x80065f60((_DAT_800aca88 - puVar6[0xd]) * (_DAT_800aca88 - puVar6[0xd]) +
                          (_DAT_800aca90 - puVar6[0xf]) * (_DAT_800aca90 - puVar6[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar3;
  (**(code **)(&LAB_8011eaac + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011eaec + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012974(4000);
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  puVar6 = _DAT_800ac784;
  if (*(short *)((int)_DAT_800ac784 + 0x1da) != 0) {
    sVar2 = (short)_DAT_800ac784[0x77] + 1;
    *(short *)(_DAT_800ac784 + 0x77) = sVar2;
    *(char *)((int)puVar6 + 5) = (char)sVar2;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    if ((DAT_800acae7 != '\0') &&
       (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
      uVar5 = func_0x8001af20();
      if ((uVar5 & 1) != 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        FUN_8010d1a4();
        return;
      }
    }
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}


