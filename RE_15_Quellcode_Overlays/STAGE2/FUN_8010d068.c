/* FUN_8010d068 @ 0x8010d068  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d068(undefined4 param_1,int param_2)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 != 1) {
    if (1 < uVar4) {
      uVar2 = 4;
      if (uVar4 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 2;
        *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 100;
        return;
      }
      goto LAB_8010d9fc;
    }
    uVar2 = 1;
    if (uVar4 != 0) goto LAB_8010d9fc;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  param_2 = 0;
  if (3 < *(byte *)(_DAT_800ac784 + 0x95)) {
    iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffba,
                            (int)*(short *)(_DAT_800ac784 + 0x1ba));
    uVar4 = _DAT_800ac784;
    uVar2 = 2;
    if (iVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
LAB_8010d9fc:
      if (uVar4 == uVar2) {
        if (*(char *)(_DAT_800ac784 + 7) == '\0') {
          *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x32;
          if (-0x4b0 < *(int *)(_DAT_800ac784 + 0x38)) {
            *(undefined4 *)(_DAT_800ac784 + 0x38) = 0xfffffb50;
            *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
          }
        }
        else {
          cVar1 = func_0x8001c0dc(800,400,0xfffffff6);
          *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
          *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x32;
          iVar3 = (*(byte *)(_DAT_800ac784 + 0x82) + 1) * -0x708;
          if (*(int *)(_DAT_800ac784 + 0x38) < iVar3) {
            *(int *)(_DAT_800ac784 + 0x38) = iVar3;
            FUN_8010e3dc();
            return;
          }
        }
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
        func_0x800245d8(0);
        FUN_8010e510();
        return;
      }
      if (uVar4 != 3) {
        FUN_8010e510();
        return;
      }
      if (*(char *)(param_2 + 7) != '\0') {
        *(char *)(param_2 + 0x82) = *(char *)(param_2 + 0x82) + '\x01';
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
      return;
    }
  }
  return;
}


