/* FUN_8010e728 @ 0x8010e728  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e728(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int in_v1;
  
  *(short *)(*(int *)(in_v1 + -0x387c) + 0x1ba) =
       *(short *)(*(int *)(in_v1 + -0x387c) + 0x1ba) + -0x708;
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),param_3,0x200);
  if (iVar1 != 0) {
    *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xc1c;
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    func_0x800245d8(0);
    *(undefined2 *)(_DAT_800ac784 + 0x40) = *(undefined2 *)(_DAT_800ac784 + 0x34);
    *(undefined2 *)(_DAT_800ac784 + 0x44) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
    *(undefined2 *)(_DAT_800ac784 + 0x42) = *(undefined2 *)(_DAT_800ac784 + 0x38);
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    *(undefined1 *)(_DAT_ffffc788 + 0x93) = 0;
    return;
  }
  return;
}


