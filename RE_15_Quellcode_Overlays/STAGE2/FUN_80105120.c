/* FUN_80105120 @ 0x80105120  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105120(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint unaff_s1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee836c))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_801068cc();
  }
  else {
    FUN_80106734();
  }
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_80117f34 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  if (iVar2 == 0) {
LAB_80105c9c:
    FUN_80109230(0,0);
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40);
    uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
    if (uVar3 == unaff_s1) {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40);
      uVar3 = (uint)*(byte *)(_DAT_800ac784 + 6);
    }
    if (uVar3 == 0) {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198);
    }
    func_0x8001af20();
    func_0x800453d0(5);
    puVar4 = &DAT_800b0fe0;
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    if (_DAT_800b0fe0 < 3) {
      FUN_80109230(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
  }
  else {
    if (*(char *)(_DAT_800ac784 + 6) != '\0') goto LAB_80105c9c;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_80117f88);
    puVar4 = (undefined1 *)0x0;
  }
  func_0x8004ef90(puVar4 + 0x78);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80106d34();
  return;
}


