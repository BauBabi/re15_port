/* FUN_8010d9c0 @ 0x8010d9c0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d9c0(void)

{
  byte bVar1;
  int iVar2;
  char cVar3;
  short sVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        FUN_8010e510();
        return;
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      sVar4 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
      *(short *)(_DAT_800ac784 + 0x6a) = sVar4 + *(short *)(_DAT_800ac784 + 0x6a);
      if (sVar4 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 1;
        FUN_8010e510();
        return;
      }
    }
    else {
      if (bVar1 == 2) goto LAB_8010dae4;
      if (bVar1 != 3) {
        FUN_8010e510();
        return;
      }
      if (*(char *)(_DAT_800ac784 + 7) != '\0') {
        *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
        FUN_8010e510();
        return;
      }
      func_0x8001c0dc(0x898,800,3000);
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0xfb50;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  if (*(char *)(_DAT_800ac784 + 7) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
  }
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
LAB_8010dae4:
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x32;
    if (-0x4b0 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xfffffb50;
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    }
  }
  else {
    cVar3 = func_0x8001c0dc(800,400,0xfffffff6);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar3;
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x32;
    iVar2 = (*(byte *)(_DAT_800ac784 + 0x82) + 1) * -0x708;
    if (*(int *)(_DAT_800ac784 + 0x38) < iVar2) {
      *(int *)(_DAT_800ac784 + 0x38) = iVar2;
      FUN_8010e3dc();
      return;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  FUN_8010e510();
  return;
}


