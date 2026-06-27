/* FUN_80117f80 @ 0x80117f80  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117f80(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  *(undefined1 *)(_DAT_800ac784 + 0x1dc) = 9;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x101;
        *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) | 0x10;
        *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) & 0xf7;
      }
      goto LAB_801188cc;
    }
    if (bVar1 != 0) goto LAB_801188cc;
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
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x95);
  if (((cVar2 == '`') || (cVar2 == '<')) || (cVar2 == 'x')) {
    func_0x800453d0(8);
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == -0x6a) {
    func_0x800453d0(9);
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x6e) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x736) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 3;
    func_0x8003a95c(iVar4 + 0x6b8,1);
    *(undefined2 *)(iVar4 + 0x736) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x7e2) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -3;
    func_0x8003a95c(iVar4 + 0x764,1);
    *(undefined2 *)(iVar4 + 0x7e2) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x88e) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x810,1);
    *(undefined2 *)(iVar4 + 0x88e) = 0xfff;
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
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x736) = 0;
    func_0x8003a95c(iVar4 + 0x6b8,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x7e2) = 0;
    func_0x8003a95c(iVar4 + 0x764,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined2 *)(iVar4 + 0x88e) = 0;
    func_0x8003a95c(iVar4 + 0x810,1);
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x532) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x4b4,1);
    *(undefined2 *)(iVar4 + 0x532) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x5de) =
         *(short *)(iVar4 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar4 + 0x560,1);
    *(undefined2 *)(iVar4 + 0x5de) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x68a) =
         *(short *)(iVar4 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x60c,1);
    *(undefined2 *)(iVar4 + 0x68a) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x93a) =
         *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar4 + 0x8bc,1);
    *(undefined2 *)(iVar4 + 0x93a) = 0xfff;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x9e6) =
         *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x968,1);
    *(undefined2 *)(iVar4 + 0x9e6) = 0xfff;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    func_0x8003a95c(iVar4 + 0x4b4,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x5de) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar5 + 0x560,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x68a) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar5 + 0x60c,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x93a) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar5 + 0x8bc,1);
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar5 + 0x9e6) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar5 + 0x968,1);
    *(short *)(iVar4 + 0x532) = *(short *)(iVar4 + 0x532) + -0x88;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x532) =
         *(short *)(iVar4 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar4 + 0x4b4,1);
    *(undefined2 *)(iVar4 + 0x532) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x5de) = *(short *)(iVar4 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar4 + 0x560,1);
    *(undefined2 *)(iVar4 + 0x5de) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x68a) = *(short *)(iVar4 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar4 + 0x60c,1);
    *(undefined2 *)(iVar4 + 0x68a) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x93a) =
         *(short *)(iVar4 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
    func_0x8003a95c(iVar4 + 0x8bc,1);
    *(undefined2 *)(iVar4 + 0x93a) = 0;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar4 + 0x9e6) = *(short *)(iVar4 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
    func_0x8003a95c(iVar4 + 0x968,1);
    *(undefined2 *)(iVar4 + 0x9e6) = 0;
  }
  *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
LAB_801188cc:
  FUN_8011b5c4(0,1);
  return;
}


