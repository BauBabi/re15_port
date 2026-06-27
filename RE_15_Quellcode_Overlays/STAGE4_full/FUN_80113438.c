/* FUN_80113438 @ 0x80113438  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113438(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  undefined1 uVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x10) == 0) {
      iVar5 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4;
    }
    else {
      iVar5 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) * 0x10 + 0x800;
    }
    sVar2 = func_0x8001aa68(iVar5,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar2 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    }
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    bVar1 = *(byte *)(_DAT_800ac784 + 9);
    if (bVar1 == 8) {
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x2f;
    }
    else if (bVar1 < 9) {
      if (bVar1 == 0) {
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x2d;
        uVar6 = 0;
LAB_80113674:
        func_0x800245d8(uVar6);
      }
    }
    else if (bVar1 == 9) {
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x2f;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x640;
    }
    else if (bVar1 == 0x10) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x2f;
      uVar6 = 0x800;
      goto LAB_80113674;
    }
    cVar3 = *(char *)(_DAT_800ac784 + 0x95);
    if (cVar3 == '\n') {
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_8011a0b0;
      cVar3 = *(char *)(_DAT_800ac784 + 0x95);
    }
    if (cVar3 == '#') {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x12) {
      FUN_80115bec(0,0);
    }
    else {
      FUN_80115bec(0,1);
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + -0x400;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    bVar1 = *(byte *)(_DAT_800ac784 + 9);
    if (bVar1 == 8) {
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_8011a0ac;
      *(undefined1 *)(_DAT_800ac784 + 9) = 0x10;
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xffffeae8;
      uVar7 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar7 != 1) {
        if (uVar7 < 2) {
          if (uVar7 != 0) {
            return;
          }
LAB_80113968:
          *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x671;
          return;
        }
        if (uVar7 == 2) goto LAB_80113940;
        if (uVar7 != 3) {
          return;
        }
        goto LAB_80113954;
      }
    }
    else {
      if (bVar1 < 9) {
        if (bVar1 != 0) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 9) = 8;
        *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x7de;
        return;
      }
      if (bVar1 != 9) {
        if (bVar1 != 0x10) {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 9) = 9;
        *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x671;
        return;
      }
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_8011a0ac;
      *(undefined1 *)(_DAT_800ac784 + 9) = 1;
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      uVar7 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar7 == 1) {
LAB_80113954:
        *(int *)(_DAT_800ac784 + 0x3c) = *(int *)(_DAT_800ac784 + 0x3c) + -0x671;
        return;
      }
      if (uVar7 < 2) {
        if (uVar7 != 0) {
          return;
        }
LAB_80113940:
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x671;
        return;
      }
      if (uVar7 == 2) goto LAB_80113968;
      if (uVar7 != 3) {
        return;
      }
    }
    *(int *)(_DAT_800ac784 + 0x3c) = *(int *)(_DAT_800ac784 + 0x3c) + 0x671;
    break;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
      uVar4 = 1;
      break;
    default:
      goto switchD_80113470_default;
    case 8:
    case 9:
      uVar4 = 0xc;
      break;
    case 0x10:
      uVar4 = 0xd;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar4;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
switchD_80113470_default:
  return;
}


