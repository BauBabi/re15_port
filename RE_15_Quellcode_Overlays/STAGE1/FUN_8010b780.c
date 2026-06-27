/* FUN_8010b780 @ 0x8010b780  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b780(undefined1 param_1)

{
  int extraout_v1;
  int extraout_v1_00;
  int iVar1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
      FUN_80106a38();
      iVar1 = extraout_v1_00;
    }
    else {
      FUN_801068a0();
      iVar1 = extraout_v1;
    }
    *(undefined2 *)(iVar1 + 0x9c) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = param_1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    FUN_8010c8f8();
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedfc64))();
  return;
}


