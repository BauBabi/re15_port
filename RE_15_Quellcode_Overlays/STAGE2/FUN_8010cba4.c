/* FUN_8010cba4 @ 0x8010cba4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010cba4(void)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  if (*(char *)(_DAT_800ac784 + 0x1e0) == '\x01') {
    if (((DAT_800acae7 == '\0') &&
        (iVar4 = func_0x8001a804(6000,0x180,&DAT_800aca88), uVar5 = _DAT_800ac784, iVar4 < 0)) &&
       (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar6 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      goto code_r0x8010d674;
    }
    if ((5000 < *(ushort *)(_DAT_800ac784 + 0x1d4)) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      uVar5 = func_0x8001af20();
      uVar6 = func_0x8001af20();
      if ((uVar5 & 1) * (uVar6 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 6;
        return;
      }
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      uVar5 = (uint)*(byte *)(_DAT_800ac784 + 0x82);
      uVar6 = uVar5 * -0x708;
      *(short *)(_DAT_800ac784 + 0x1ba) = (short)uVar6;
      goto code_r0x8010d674;
    }
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if (((bVar1 & 3) != 0) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      uVar5 = _DAT_800ac784;
      uVar6 = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      goto code_r0x8010d674;
    }
  }
  sVar3 = func_0x8001a9cc(&DAT_800aca88,1000);
  *(short *)(_DAT_800ac784 + 0x1da) = sVar3;
  if ((sVar3 == 0) ||
     ((uVar2 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
      (uVar2 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
    if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  uVar5 = _DAT_800ac784;
  uVar6 = (uint)*(ushort *)(_DAT_800ac784 + 0x1da);
  *(ushort *)(_DAT_800ac784 + 0x9c) = *(ushort *)(_DAT_800ac784 + 0x1da);
code_r0x8010d674:
  *(short *)(uVar5 + 0x6a) = (short)uVar6 + 0x800;
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


