/* FUN_8011a1c8 @ 0x8011a1c8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a1c8(void)

{
  undefined2 uVar1;
  int iVar2;
  ushort extraout_var;
  int in_v1;
  undefined4 in_stack_00000010;
  undefined4 in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  *(undefined1 *)(*(int *)(in_v1 + -0x387c) + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1c;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x800453d0(7);
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\x04') &&
     (iVar2 = func_0x8001a780(&DAT_800aca54), iVar2 == 0)) {
    in_stack_00000010 = _DAT_80072d60;
    in_stack_00000014 = _DAT_80072d64;
    in_stack_00000018 = _DAT_80072d68;
    in_stack_0000001c = _DAT_80072d6c;
    func_0x8001a804(3000,0x100,&DAT_800aca88);
    *(ushort *)(_DAT_800ac784 + 0x1d8) = extraout_var >> 0xf;
    if ((DAT_800acae7 != '\0') || (*(short *)(_DAT_800ac784 + 0x1d8) == 0)) goto LAB_8011a2f0;
LAB_8011a3ac:
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  else {
LAB_8011a2f0:
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
      return;
    }
    iVar2 = func_0x8001a780(&DAT_800aca54);
    if (iVar2 != 0) {
      in_stack_00000010 = _DAT_80072d60;
      in_stack_00000014 = _DAT_80072d64;
      in_stack_00000018 = _DAT_80072d68;
      in_stack_0000001c = _DAT_80072d6c;
      uVar1 = func_0x8001bff8(*(int *)(_DAT_800ac784 + 0x188) + 0x39c,&stack0x00000010,800,
                              &DAT_800aca88);
      *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar1;
      if ((DAT_800acae7 == '\0') && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) goto LAB_8011a3ac;
    }
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
  }
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x800453d0();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}


