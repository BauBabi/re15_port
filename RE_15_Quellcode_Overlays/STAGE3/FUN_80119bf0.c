/* FUN_80119bf0 @ 0x80119bf0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119bf0(void)

{
  short sVar1;
  char cVar2;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_20 = DAT_80100068;
  local_1c = DAT_8010006c;
  local_18 = DAT_80100070;
  local_14 = DAT_80100074;
  if ((*(char *)(_DAT_800ac784 + 7) == '\0') && ((*(byte *)(_DAT_800ac784 + 0x1dd) & 8) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1de) = 9;
    sVar1 = func_0x8001a6d4(*(undefined4 *)(_DAT_800ac784 + 0x34),
                            *(undefined4 *)(_DAT_800ac784 + 0x3c),_DAT_800aca88,_DAT_800aca90);
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x198,&local_20);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\x01') {
      func_0x80019700(0x2000,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x5a0,&local_20);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\x02') {
      func_0x80019700(0x2000,(int)sVar1,*(int *)(_DAT_800ac784 + 0x188) + 0x64c,&local_20);
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0x32;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
    *(undefined4 *)(_DAT_800ac784 + 4) = *(undefined4 *)(_DAT_800ac784 + 0x1d8);
    FUN_8011a5c0();
    return;
  }
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0e1c))();
  return;
}


