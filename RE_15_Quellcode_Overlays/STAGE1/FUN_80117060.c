/* FUN_80117060 @ 0x80117060  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117060(void)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  ushort uVar4;
  uint uVar5;
  uint in_v1;
  uint uVar6;
  undefined8 uVar7;
  
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 6);
  uVar5 = (uint)(uVar6 < 2);
  if (uVar6 == 1) {
LAB_80117130:
    if ((*(char *)(_DAT_800ac784 + 7) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x1d')) {
      func_0x800453d0(4);
    }
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    uVar6 = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    uVar7 = FUN_8011bf50(0,0);
    in_v1 = (uint)((ulonglong)uVar7 >> 0x20);
    uVar5 = (uint)uVar7;
  }
  else {
    if (uVar5 == 0) {
      uVar5 = 2;
      if (uVar6 != 2) goto LAB_80117a2c;
      in_v1 = (uint)*(byte *)(_DAT_800ac784 + 7);
      if (in_v1 == 1) {
        uVar2 = 4;
      }
      else {
        if (1 < in_v1) {
          uVar5 = 3;
          if (in_v1 == 2) {
            *(undefined1 *)(_DAT_800ac784 + 5) = 3;
            *(undefined1 *)(_DAT_800ac784 + 6) = 0;
            *(undefined1 *)(_DAT_800ac784 + 7) = 1;
            return;
          }
          goto LAB_80117a2c;
        }
        uVar5 = 3;
        uVar2 = 3;
        if (in_v1 != 0) goto LAB_80117a2c;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar5 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto LAB_80117a2c;
    }
    if (uVar6 != 0) goto LAB_80117a2c;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar5 = (uint)*(byte *)(_DAT_800ac784 + 7);
    if (uVar5 == 1) {
      uVar2 = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    }
    else if (uVar5 < 2) {
      uVar2 = 3;
      if (uVar5 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      }
    }
    else {
      uVar2 = 2;
      if (uVar5 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
        goto LAB_80117130;
      }
    }
    *(undefined1 *)(uVar5 + 6) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x1f) + 0xb4;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar3 & 0x1f) + 0x40;
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
      *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
    }
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x95);
    in_v1 = uVar6 / 0xd;
    uVar5 = in_v1 * 3;
  }
LAB_80117a2c:
  if ((uVar6 - (uVar5 * 4 + in_v1) & 0xff) == 1) {
    func_0x800453d0(8);
  }
  func_0x800245d8(0);
  return;
}


