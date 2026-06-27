/* FUN_8010d3a4 @ 0x8010d3a4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d3a4(void)

{
  byte bVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  if (*(char *)(_DAT_800ac784 + 0x1e0) == '\x01') {
    if (((DAT_800acae7 == '\0') && (iVar4 = func_0x8001a804(6000,0x180,&DAT_800aca88), iVar4 < 0))
       && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      return;
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1389) ||
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
LAB_8010d4a0:
      if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
         ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
        return;
      }
      bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
      if (((bVar1 & 3) == 0) ||
         (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff))
         ) goto LAB_8010d5a4;
      *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    }
    else {
      uVar5 = func_0x8001af20();
      uVar6 = func_0x8001af20();
      if ((uVar5 & 1) * (uVar6 & 1) == 0) goto LAB_8010d4a0;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  else {
LAB_8010d5a4:
    sVar3 = func_0x8001a9cc(&DAT_800aca88,1000);
    *(short *)(_DAT_800ac784 + 0x1da) = sVar3;
    if ((sVar3 == 0) ||
       ((uVar2 = 9, *(short *)(_DAT_800ac784 + 0x1dc) != 0 &&
        (uVar2 = 8, *(char *)(_DAT_800ac784 + 0x1e3) != '\0')))) {
      if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
         ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
        *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 0;
      }
    }
    else {
      *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x1da);
    }
  }
  return;
}


