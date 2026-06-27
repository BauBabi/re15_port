/* FUN_801074b0 @ 0x801074b0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801074b0(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 *unaff_s0;
  int unaff_s1;
  int unaff_s2;
  undefined4 in_stack_00000028;
  undefined4 in_stack_0000002c;
  undefined4 in_stack_00000030;
  undefined4 in_stack_00000034;
  undefined4 in_stack_00000038;
  undefined4 in_stack_0000003c;
  undefined4 in_stack_00000040;
  undefined4 in_stack_00000044;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\x01') {
LAB_801075cc:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\a') {
      uVar3 = func_0x8001af20();
      uVar4 = 8;
      if ((uVar3 & 1) != 0) {
        uVar4 = 5;
      }
      func_0x800453d0(uVar4);
    }
    if (*(char *)(_DAT_800ac784 + 0x95) == '#') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011f784)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
    }
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    iVar2 = _DAT_800ac784;
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  }
  else {
    unaff_s0 = (undefined4 *)0x1;
    iVar2 = _DAT_800ac784;
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
      func_0x80019700(0x2500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_8012016c);
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
      if (2 < _DAT_800b0fe0) {
        func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        goto LAB_801075cc;
      }
      *(int *)(unaff_s1 + 0x50c) = _DAT_800ac784;
      *(undefined2 *)(unaff_s1 + 0x4ee) = 0x1038;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -200 < *(int *)(unaff_s1 + 0x50c)) {
        *(undefined2 *)(unaff_s1 + 0x4ee) = 0xfbf2;
        *(int *)(unaff_s1 + 0x50c) = *(short *)(_DAT_800ac784 + 0x1ba) + -200;
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),unaff_s1 + 0x4f4,&DAT_8012016c)
        ;
        if (*(char *)(unaff_s1 + 0x529) != '\0') {
          *(char *)(unaff_s1 + 0x529) = *(char *)(unaff_s1 + 0x529) + -1;
        }
      }
      if (*(short *)(unaff_s2 + 0x82) == 0) {
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x560) | 0x200;
        *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) =
             *(uint *)(*(int *)(_DAT_800ac784 + 0x188) + 0x764) | 0x200;
        goto LAB_8010877c;
      }
      *(short *)(unaff_s2 + 0x82) = *(short *)(unaff_s2 + 0x82) + -1;
      in_stack_00000028 = _DAT_80072d4c;
      in_stack_0000002c = _DAT_80072d50;
      in_stack_00000030 = _DAT_80072d54;
      in_stack_00000034 = _DAT_80072d58;
      in_stack_00000038 = _DAT_80072d5c;
      in_stack_0000003c = _DAT_80072d60;
      in_stack_00000040 = _DAT_80072d64;
      in_stack_00000044 = _DAT_80072d68;
      unaff_s0 = &stack0x00000028;
      func_0x80065b70(0x80,unaff_s0);
      iVar2 = unaff_s2 + 0x40;
    }
  }
  func_0x80022da0(iVar2);
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) == 0) {
    iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar2;
    if (iVar2 == 0) {
      uVar3 = func_0x8001af20();
      uVar4 = 5;
      if ((uVar3 & 1) != 0) {
        uVar4 = 4;
      }
      func_0x800453d0(uVar4);
    }
    func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8)
    ;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x100);
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
      FUN_8010939c(0,0);
      return;
    }
    FUN_801092dc(0,1);
    return;
  }
  func_0x80019d50(0x2000);
LAB_8010877c:
  func_0x80019d50();
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0857) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0856) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x16,(uint)*(byte *)((int)unaff_s0 + -0x7fee0855) * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  if (2 < _DAT_800b0fe0) {
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    FUN_80109164();
    return;
  }
  FUN_80109100(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  return;
}


