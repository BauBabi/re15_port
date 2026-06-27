/* FUN_80117780 @ 0x80117780  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117780(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  int unaff_s0;
  int iVar6;
  undefined4 uVar7;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  *(undefined1 *)(_DAT_800ac784 + 0x1dc) = 9;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  uVar4 = (uint)(bVar1 < 2);
  if (bVar1 == 1) {
LAB_801178b8:
    cVar2 = *(char *)(_DAT_800ac784 + 0x95);
    if (((cVar2 == '`') || (cVar2 == '<')) || (cVar2 == 'x')) {
      func_0x800453d0(8);
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == -0x6a) {
      func_0x800453d0(9);
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x6e) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x736) =
           *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 3;
      func_0x8003a95c(iVar5 + 0x6b8,1);
      *(undefined2 *)(iVar5 + 0x736) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x7e2) =
           *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -3;
      func_0x8003a95c(iVar5 + 0x764,1);
      *(undefined2 *)(iVar5 + 0x7e2) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x88e) =
           *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x810,1);
      *(undefined2 *)(iVar5 + 0x88e) = 0xfff;
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0xb < 10) {
        *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
      }
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0x3d < 10) {
        *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
      }
      if (*(byte *)(_DAT_800ac784 + 0x95) - 0x61 < 10) {
        *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0xca;
      }
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == 'n') {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x736) = 0;
      func_0x8003a95c(iVar5 + 0x6b8,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x7e2) = 0;
      func_0x8003a95c(iVar5 + 0x764,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar5 + 0x88e) = 0;
      func_0x8003a95c(iVar5 + 0x810,1);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x532) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x4b4,1);
      *(undefined2 *)(iVar5 + 0x532) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar5 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x560,1);
      *(undefined2 *)(iVar5 + 0x5de) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x60c,1);
      *(undefined2 *)(iVar5 + 0x68a) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      *(undefined2 *)(iVar5 + 0x93a) = 0xfff;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(undefined2 *)(iVar5 + 0x9e6) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      func_0x8003a95c(iVar5 + 0x4b4,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x5de) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x560,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x68a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x60c,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x93a) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x8bc,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x9e6) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x968,1);
      *(short *)(iVar5 + 0x532) = *(short *)(iVar5 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x532) =
           *(short *)(iVar5 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x4b4,1);
      *(undefined2 *)(iVar5 + 0x532) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar5 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x560,1);
      *(undefined2 *)(iVar5 + 0x5de) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar5 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x60c,1);
      *(undefined2 *)(iVar5 + 0x68a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar5 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      *(undefined2 *)(iVar5 + 0x93a) = 0;
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar5 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(undefined2 *)(iVar5 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_801188c8();
    return;
  }
  if (uVar4 == 0) {
    uVar4 = 0x101;
    if (bVar1 == 2) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
      *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
      *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) & 0xf7;
      FUN_8011b5c4(0,1);
      return;
    }
  }
  else if (bVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 1;
    func_0x8004ef90(0x800b1028,0x1d);
    func_0x800453d0(1);
    func_0x800453d0(8);
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0x96;
    *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) | 8;
    goto LAB_801178b8;
  }
  iVar5 = (uint)*(ushort *)(unaff_s0 + 0xa92) - (uint)*(ushort *)(uVar4 + 0x9c);
  *(short *)(unaff_s0 + 0xa92) = (short)iVar5;
  if (iVar5 * 0x10000 < 0) {
    *(undefined2 *)(unaff_s0 + 0xa92) = 0;
  }
  func_0x8003a95c(unaff_s0 + 0xa14,1);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
  if (*(short *)(unaff_s0 + 0xa92) < 1) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
  }
  iVar5 = *(int *)(_DAT_800ac784 + 0x84);
  uVar7 = 0x80118950;
  func_0x8001f314(iVar5,*(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  bVar1 = *(byte *)(iVar5 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(iVar5 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) | 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1d4) = 0xaa10;
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xd120;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x60
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200,uVar7);
  if (*(uint *)(_DAT_800ac784 + 0x1d0) < 500) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  FUN_8011b5c4(1,1);
  iVar5 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar5 >> 0x10) - (iVar5 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}


