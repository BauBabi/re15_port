/* FUN_801130a4 @ 0x801130a4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801130a4(void)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e0c;
  case 1:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
      iVar2 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) * 0x10 + 0x800;
    }
    else {
      iVar2 = ((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4;
    }
    sVar1 = func_0x8001aa68(iVar2,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar1 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
      func_0x800245d8(0);
      break;
    case 2:
      func_0x800245d8(0x800);
      break;
    case 3:
      if (-0x2a30 < *(int *)(_DAT_800ac784 + 0x38)) {
        uVar3 = func_0x8001af20();
        iVar2 = (*(int *)(_DAT_800ac784 + 0x38) + -100) - (uVar3 & 0xf);
LAB_80113364:
        *(int *)(_DAT_800ac784 + 0x38) = iVar2;
      }
      break;
    case 4:
      if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -500) {
        uVar3 = func_0x8001af20();
        iVar2 = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar3 & 0xf);
        goto LAB_80113364;
      }
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x1f') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    break;
  case 4:
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
      *(undefined1 *)(_DAT_800ac784 + 9) = 3;
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x5d0;
      break;
    case 2:
      *(undefined1 *)(_DAT_800ac784 + 9) = 4;
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x5d0;
      break;
    case 3:
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e04;
      *(undefined1 *)(_DAT_800ac784 + 9) = 2;
      *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xffffd5d0;
      uVar3 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar3 == 1) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + 0x5d0;
LAB_80113500:
        *(int *)(_DAT_800ac784 + 0x3c) = iVar2;
      }
      else if (uVar3 < 2) {
        if (uVar3 == 0) {
          *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x5d0;
        }
      }
      else if (uVar3 == 2) {
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x5d0;
      }
      else if (uVar3 == 3) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + -0x5d0;
        goto LAB_80113500;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0x3c;
      break;
    case 4:
      *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e04;
      *(undefined1 *)(_DAT_800ac784 + 9) = 5;
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      uVar3 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
      if (uVar3 == 1) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + -0x5d0;
LAB_801135f8:
        *(int *)(_DAT_800ac784 + 0x3c) = iVar2;
      }
      else if (uVar3 < 2) {
        if (uVar3 == 0) {
          *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x5d0;
        }
      }
      else if (uVar3 == 2) {
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x5d0;
      }
      else if (uVar3 == 3) {
        iVar2 = *(int *)(_DAT_800ac784 + 0x3c) + 0x5d0;
        goto LAB_801135f8;
      }
    }
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + -0x400;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    break;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    switch(*(undefined1 *)(_DAT_800ac784 + 9)) {
    case 0:
    case 1:
    case 2:
    case 5:
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0;
      break;
    case 3:
    case 4:
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xd;
    }
  }
  return;
}


