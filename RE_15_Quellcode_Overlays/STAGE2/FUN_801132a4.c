/* FUN_801132a4 @ 0x801132a4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801132a4(void)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined4 unaff_s0;
  int unaff_s1;
  int iStackX_c;
  int in_stack_00000014;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 7);
  uVar3 = (uint)(uVar4 < 2);
  if (uVar4 != 1) {
    uVar6 = _DAT_800ac784;
    if (uVar3 == 0) {
      uVar3 = 1;
      if (uVar4 == 2) {
        *(undefined2 *)(_DAT_800ac784 + 4) = 1;
        *(byte *)(_DAT_800ac784 + 6) = (*(byte *)(_DAT_800ac784 + 9) & 1) + 3;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
        *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 1;
          *(undefined1 *)(_DAT_800ac784 + 6) = 10;
        }
        return;
      }
      goto LAB_80113f2c;
    }
    if (uVar4 != 0) goto LAB_80113f2c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118f18);
    func_0x800453d0(2);
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) {
      uVar2 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x1e0) = uVar2 & 7;
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 8;
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
      sVar1 = *(short *)(_DAT_800ac784 + 0x1e4);
      *(short *)(_DAT_800ac784 + 0x1e4) = sVar1 + -1;
      while (sVar1 != 0) {
        if (*(char *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) ==
            *(char *)(*(short *)(_DAT_800ac784 + 0x1e4) + -0x7fee70d0)) {
          *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 1;
        }
        sVar1 = *(short *)(_DAT_800ac784 + 0x1e4);
        *(short *)(_DAT_800ac784 + 0x1e4) = sVar1 + -1;
      }
      if (*(short *)(_DAT_800ac784 + 0x1e2) == 0) {
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1de) + -0x7fee70d0) =
             *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8);
        puVar5 = (uint *)((uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                         *(int *)(_DAT_800ac784 + 0x188));
        puVar5[0x1a] = 0x207f60;
        *(undefined2 *)(puVar5 + 0x25) = 0;
        *(undefined2 *)((int)puVar5 + 0x96) = 0xffce;
        *(undefined2 *)(puVar5 + 0x26) = 0;
        *(undefined2 *)((int)puVar5 + 0x9a) = 3;
        *(undefined2 *)(puVar5 + 0x27) = 0;
        *(undefined2 *)((int)puVar5 + 0x9e) = 0;
        *puVar5 = *puVar5 | 0x4a;
        unaff_s0 = 0x80118f18;
        iVar8 = (uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                *(int *)(_DAT_800ac784 + 0x188);
        *(undefined4 *)(iVar8 + 0x114) = 0x207f60;
        *(undefined2 *)(iVar8 + 0x140) = 0;
        *(undefined2 *)(iVar8 + 0x142) = 0xffce;
        *(undefined2 *)(iVar8 + 0x144) = 0;
        *(undefined2 *)(iVar8 + 0x146) = 3;
        *(undefined2 *)(iVar8 + 0x148) = 0;
        *(undefined2 *)(iVar8 + 0x14a) = 0;
        *(uint *)(iVar8 + 0xac) = *(uint *)(iVar8 + 0xac) | 0x4a;
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                        0x198,0x80118f18);
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)*(byte *)(*(short *)(_DAT_800ac784 + 0x1e0) + -0x7fee70d8) * 0xac +
                        0x198,0x80118f18);
        func_0x800453d0(5);
        *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
      }
    }
  }
  uVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar6 = _DAT_800ac784;
  uVar4 = *(byte *)(_DAT_800ac784 + 7) + uVar3;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar4;
LAB_80113f2c:
  *(undefined2 *)(uVar3 + 0xd62) = 0;
  *(uint *)(uVar3 + 0xcc4) = uVar4 | 0x4a;
  func_0x80019700(uVar6,(int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,unaff_s0);
  func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0xd04,unaff_s0);
  uVar3 = _DAT_800ac784;
  *(undefined2 *)(_DAT_800ac784 + 0x1e0) = 9;
  sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
  iVar8 = *(int *)(uVar3 + 0x188);
  *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  while (sVar1 != 1) {
    iVar7 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8;
    uVar3 = *(uint *)(iVar7 + 0xac);
    if ((uVar3 & 0x41) == 1) {
      *(uint *)(iVar7 + 0xac) = uVar3 | 0x1062;
      iVar7 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8;
      *(uint *)(iVar7 + 0x158) = *(uint *)(iVar7 + 0x158) | 0x1062;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x121) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x12e) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x122) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0xe4) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0xe6) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0xe8) = 0;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x1cd) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x1da) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x1ce) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 400) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x192) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar8 + 0x194) = 0;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  iVar8 = _DAT_0000018a;
  func_0x80022da0(*(int *)(uVar3 - 0x387c) + 0x20,_DAT_0000018a + 0x18,&stack0xfffffff8);
  iVar8 = iVar8 + unaff_s1 * 0x204 + 0xe1c;
  func_0x80022da0(&stack0xfffffff8,iVar8 + -0x140,&stack0xfffffff8);
  func_0x80022da0(&stack0xfffffff8,iVar8 + -0x94,&stack0xfffffff8);
  func_0x80022da0(&stack0xfffffff8,iVar8 + 0x18,&stack0xfffffff8);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (iStackX_c - *(int *)(iVar8 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (in_stack_00000014 - *(int *)(iVar8 + 0x5c));
  return;
}


