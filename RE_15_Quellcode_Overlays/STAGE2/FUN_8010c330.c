/* FUN_8010c330 @ 0x8010c330  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c330(void)

{
  byte bVar1;
  byte bVar2;
  undefined1 uVar3;
  ushort uVar4;
  short sVar5;
  uint uVar6;
  uint uVar7;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  bVar2 = bVar1 < 2;
  if (bVar1 != 1) {
    if (!(bool)bVar2) {
      bVar2 = 0;
      if (bVar1 == 2) {
        sVar5 = *(short *)(_DAT_800ac784 + 0x9c);
        *(short *)(_DAT_800ac784 + 0x9c) = sVar5 + -1;
        if (sVar5 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        return;
      }
      goto code_r0x8010cc5c;
    }
    if (bVar1 != 0) goto code_r0x8010cc5c;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x7f) + 0x3b;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  bVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(byte *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + bVar2;
code_r0x8010cc5c:
  if ((bVar2 & 1) != 0) {
    uVar6 = func_0x8001af20();
    uVar7 = func_0x8001af20();
    if ((uVar6 & 1) * (uVar7 & 1) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
      return;
    }
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) && ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)
     ) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    uVar6 = (uint)*(byte *)(_DAT_800ac784 + 0x82);
    sVar5 = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(short *)(_DAT_800ac784 + 0x1ba) = sVar5;
  }
  else {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if (((bVar1 & 3) == 0) ||
       (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff)))
    {
      sVar5 = func_0x8001a9cc(&DAT_800aca88,1000);
      *(short *)(_DAT_800ac784 + 0x1da) = sVar5;
      if ((sVar5 == 0) ||
         ((uVar3 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
          (uVar3 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
        if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
           ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
          *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
        }
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      uVar6 = _DAT_800ac784;
      sVar5 = *(short *)(_DAT_800ac784 + 0x1da);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar5;
    }
    else {
      *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      sVar5 = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    }
  }
  *(short *)(uVar6 + 0x6a) = sVar5 + 0x800;
  *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
  if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}


