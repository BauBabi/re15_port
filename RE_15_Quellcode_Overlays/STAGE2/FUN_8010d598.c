/* FUN_8010d598 @ 0x8010d598  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d598(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    FUN_8010ee40((int)*(short *)(_DAT_800ac784 + 0x9c),1);
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    *(undefined1 *)(_DAT_ffffc785 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    return;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  if (iVar1 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x800;
    *(undefined1 *)(_DAT_800ac784 + 0x1e3) = 0x96;
    if ((*(char *)(_DAT_800ac784 + 0x93) != '\0') && (*(char *)(_DAT_800ac784 + 0x1e0) != '\x02')) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    }
  }
  return;
}


