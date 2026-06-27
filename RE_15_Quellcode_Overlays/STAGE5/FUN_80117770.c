/* FUN_80117770 @ 0x80117770  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117770(void)

{
  undefined2 uVar1;
  char cVar2;
  short sVar3;
  int in_v0;
  uint uVar4;
  int iVar5;
  int iVar6;
  short in_lo;
  short sStack00000018;
  short sStack0000001a;
  short in_stack_0000001c;
  int in_stack_00000020;
  int in_stack_00000024;
  int in_stack_00000028;
  char in_stack_00000030;
  
  uVar1 = *(undefined2 *)(_DAT_800ac784 + 0x6a);
  *(short *)(_DAT_800ac784 + 0x6a) = (short)in_v0 + -0x400;
  uVar4 = func_0x8001a780();
  *(undefined2 *)(_DAT_800ac784 + 0x6a) = uVar1;
  iVar6 = in_v0 - ((uVar4 & 0xff) * 0x800 + -0x400);
  _DAT_800acabe = (undefined2)iVar6;
  _DAT_800acbfc = _DAT_800ac784;
  _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
  _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  _DAT_800aca58 = (uVar4 & 0xff) << 8 | 5;
  func_0x800453d0(7);
  _DAT_800acaee = _DAT_800acaee - 10;
  if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
    _DAT_800acaee = 1;
  }
  in_stack_00000020 = (int)sStack00000018;
  in_stack_00000024 = (int)sStack0000001a;
  iVar5 = (int)in_stack_0000001c;
  in_stack_00000028 = iVar5;
  func_0x8001c2dc(&stack0x00000020,400,&stack0x00000030);
  if (in_stack_00000030 != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '%') {
    func_0x800453d0(4);
    *(short *)(iVar6 + 0x7e2) = *(short *)(iVar5 + 0x9c) + in_lo * -3;
    func_0x8003a95c();
    *(undefined2 *)(iVar6 + 0x7e2) = uVar1;
    iVar6 = *(int *)(_DAT_800ac784 + 0x188);
    sVar3 = func_0x8001af20();
    *(short *)(iVar6 + 0x88e) =
         *(short *)(_DAT_800ac784 + 0x9c) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
    func_0x8003a95c(iVar6 + 0x810,1);
    *(undefined2 *)(iVar6 + 0x88e) = uVar1;
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
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar6 + 0x736) = 0;
      func_0x8003a95c(iVar6 + 0x6b8,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar6 + 0x7e2) = 0;
      func_0x8003a95c(iVar6 + 0x764,1);
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      *(undefined2 *)(iVar6 + 0x88e) = 0;
      func_0x8003a95c(iVar6 + 0x810,1);
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x46) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x532) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x4b4,1);
      *(undefined2 *)(iVar6 + 0x532) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x5de) =
           *(short *)(iVar6 + 0x5de) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x560,1);
      *(undefined2 *)(iVar6 + 0x5de) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x68a) =
           *(short *)(iVar6 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x60c,1);
      *(undefined2 *)(iVar6 + 0x68a) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x93a) =
           *(short *)(iVar6 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x8bc,1);
      *(undefined2 *)(iVar6 + 0x93a) = 0xfff;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x9e6) =
           *(short *)(iVar6 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x968,1);
      *(undefined2 *)(iVar6 + 0x9e6) = 0xfff;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x47 < 0x1e) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      func_0x8003a95c(iVar6 + 0x4b4,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x5de) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x560,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x68a) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x60c,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x93a) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar5 + 0x8bc,1);
      iVar5 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar5 + 0x9e6) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar5 + 0x968,1);
      *(short *)(iVar6 + 0x532) = *(short *)(iVar6 + 0x532) + -0x88;
    }
    if (*(byte *)(_DAT_800ac784 + 0x95) - 0x65 < 0x32) {
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x532) =
           *(short *)(iVar6 + 0x532) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * 2;
      func_0x8003a95c(iVar6 + 0x4b4,1);
      *(undefined2 *)(iVar6 + 0x532) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x5de) =
           *(short *)(iVar6 + 0x5de) - *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar6 + 0x560,1);
      *(undefined2 *)(iVar6 + 0x5de) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x68a) =
           *(short *)(iVar6 + 0x68a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar6 + 0x60c,1);
      *(undefined2 *)(iVar6 + 0x68a) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x93a) =
           *(short *)(iVar6 + 0x93a) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3 * -2;
      func_0x8003a95c(iVar6 + 0x8bc,1);
      *(undefined2 *)(iVar6 + 0x93a) = 0;
      iVar6 = *(int *)(_DAT_800ac784 + 0x188);
      sVar3 = func_0x8001af20();
      *(short *)(iVar6 + 0x9e6) =
           *(short *)(iVar6 + 0x9e6) + *(char *)(_DAT_800ac784 + 0x9e) * sVar3;
      func_0x8003a95c(iVar6 + 0x968,1);
      *(undefined2 *)(iVar6 + 0x9e6) = 0;
    }
    *(char *)(_DAT_800ac784 + 0x9e) = -*(char *)(_DAT_800ac784 + 0x9e);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
    FUN_801190dc();
    return;
  }
  FUN_8011bdd8(0,0);
  return;
}


