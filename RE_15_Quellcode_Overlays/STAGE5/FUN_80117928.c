/* FUN_80117928 @ 0x80117928  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117928(undefined4 param_1)

{
  char cVar1;
  short sVar2;
  int in_v0;
  int iVar3;
  int unaff_s0;
  undefined2 unaff_s1;
  int iVar4;
  
  iVar3 = 0x400;
  cVar1 = func_0x8001f314(param_1,*(undefined4 *)(in_v0 + 0x16c));
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (((*(byte *)(_DAT_800ac784 + 9) & 0x10) != 0) && (*(char *)(_DAT_800ac784 + 0x95) == '\x1e')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(short *)(unaff_s0 + 0x7e2) = *(short *)(iVar3 + 0x9c) + -0x1c2;
    func_0x8003a95c();
    *(undefined2 *)(unaff_s0 + 0x7e2) = unaff_s1;
    iVar3 = *(int *)(_DAT_800ac784 + 0x188);
    sVar2 = func_0x8001af20();
    *(short *)(iVar3 + 0x88e) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
    func_0x8003a95c(iVar3 + 0x810,1);
    *(undefined2 *)(iVar3 + 0x88e) = unaff_s1;
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0xb < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x3d < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x65;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x61 < 10) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0xca;
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == 'n') {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar3 + 0x736) = 0;
      func_0x8003a95c(iVar3 + 0x6b8,1);
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar3 + 0x7e2) = 0;
      func_0x8003a95c(iVar3 + 0x764,1);
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar3 + 0x88e) = 0;
      func_0x8003a95c(iVar3 + 0x810,1);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x532) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x4b4,1);
      *(undefined2 *)(iVar3 + 0x532) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x5de) =
           *(short *)(iVar3 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar3 + 0x560,1);
      *(undefined2 *)(iVar3 + 0x5de) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x68a) =
           *(short *)(iVar3 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x60c,1);
      *(undefined2 *)(iVar3 + 0x68a) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x93a) =
           *(short *)(iVar3 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar3 + 0x8bc,1);
      *(undefined2 *)(iVar3 + 0x93a) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x9e6) =
           *(short *)(iVar3 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x968,1);
      *(undefined2 *)(iVar3 + 0x9e6) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      func_0x8003a95c(iVar3 + 0x4b4,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x5de) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar4 + 0x560,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x68a) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar4 + 0x60c,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x93a) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar4 + 0x8bc,1);
      iVar4 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar4 + 0x9e6) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar4 + 0x968,1);
      *(short *)(iVar3 + 0x532) = *(short *)(iVar3 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x532) =
           *(short *)(iVar3 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * 2;
      func_0x8003a95c(iVar3 + 0x4b4,1);
      *(undefined2 *)(iVar3 + 0x532) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x5de) =
           *(short *)(iVar3 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar3 + 0x560,1);
      *(undefined2 *)(iVar3 + 0x5de) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x68a) =
           *(short *)(iVar3 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar3 + 0x60c,1);
      *(undefined2 *)(iVar3 + 0x68a) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x93a) =
           *(short *)(iVar3 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2 * -2;
      func_0x8003a95c(iVar3 + 0x8bc,1);
      *(undefined2 *)(iVar3 + 0x93a) = 0;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x9e6) =
           *(short *)(iVar3 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar2;
      func_0x8003a95c(iVar3 + 0x968,1);
      *(undefined2 *)(iVar3 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    FUN_801190dc();
    return;
  }
  FUN_8011bdd8(0,0);
  return;
}


