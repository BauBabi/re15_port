/* FUN_8010c138 @ 0x8010c138  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c138(void)

{
  uint *puVar1;
  byte bVar2;
  char cVar3;
  ushort uVar4;
  undefined2 uVar5;
  short sVar6;
  int iVar7;
  uint uVar8;
  int in_lo;
  
  puVar1 = _DAT_800ac784;
  uVar8 = (uint)*(byte *)((int)_DAT_800ac784 + 6);
  uVar4 = (ushort)(uVar8 < 2);
  if (uVar8 != 1) {
    if (uVar8 >= 2) {
      uVar4 = 3;
      if (uVar8 != 2) {
        if (uVar8 == 3) {
          cVar3 = *(char *)((int)_DAT_800ac784 + 0x9f);
          *(char *)((int)_DAT_800ac784 + 0x9f) = cVar3 + -1;
          if (cVar3 == '\0') {
            *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
            uVar8 = _DAT_800ac784[0x74];
            *(short *)(_DAT_800ac784 + 0x74) = (short)uVar8 + -1;
            if ((short)uVar8 == 0) {
              *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
              *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
              uVar4 = func_0x8001af20();
              *(ushort *)(_DAT_800ac784 + 0x74) = (uVar4 & 0x1f) + 0x1e;
            }
          }
          func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
          func_0x800245d8(0);
          return;
        }
        goto LAB_8010cb1c;
      }
      goto LAB_8010c228;
    }
    if (uVar8 != 0) {
LAB_8010cb1c:
      *(ushort *)(uVar8 + 0x9c) = uVar4;
      *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + -0x14;
      uVar8 = _DAT_800ac784[0x23];
      if (-1 < (short)uVar8) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
      iVar7 = (int)(short)uVar8 - puVar1[0xf];
      uVar5 = func_0x80065f60(in_lo + iVar7 * iVar7);
      *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar5;
      (**(code **)(&DAT_80120f94 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
      (**(code **)(&DAT_80120fd4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
        *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
      }
      func_0x80012974(4000);
      if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
        *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
      }
      puVar1 = _DAT_800ac784;
      if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
        return;
      }
      sVar6 = (short)_DAT_800ac784[0x77] + 1;
      *(short *)(_DAT_800ac784 + 0x77) = sVar6;
      *(char *)((int)puVar1 + 5) = (char)sVar6;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
      if ((DAT_800acae7 != '\0') &&
         (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
        uVar8 = func_0x8001af20();
        if ((uVar8 & 1) != 0) {
          *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
          FUN_8010e6c4();
          return;
        }
      }
      return;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  }
  bVar2 = func_0x8001af20();
  puVar1 = _DAT_800ac784;
  cVar3 = (bVar2 & 3) + 1;
  *(char *)((int)_DAT_800ac784 + 0x9f) = cVar3;
  *(char *)((int)puVar1 + 0x9e) = cVar3;
  uVar4 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x23) = (uVar4 & 0x7f) + 0x32;
  *(undefined1 *)((int)_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  uVar8 = func_0x8001af20();
  if ((uVar8 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  }
LAB_8010c228:
  cVar3 = *(char *)((int)_DAT_800ac784 + 0x9e);
  *(char *)((int)_DAT_800ac784 + 0x9e) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  FUN_8010cb14(0x800);
  return;
}


