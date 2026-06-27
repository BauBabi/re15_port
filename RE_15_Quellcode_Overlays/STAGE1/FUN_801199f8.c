/* FUN_801199f8 @ 0x801199f8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801199f8(void)

{
  byte bVar1;
  ushort uVar2;
  undefined2 uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x18;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  *(ushort *)(_DAT_800ac784 + 0x1d8) = (ushort)*(byte *)(_DAT_800ac784 + 0x95);
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 8) ||
     (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x21 < 5)) {
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0x50;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x30) + 0x30;
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
      *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
    }
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (0x18 < *(byte *)(_DAT_800ac784 + 0x95)) {
    uVar3 = func_0x8001bff8(0x39c,0);
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar3;
    if ((DAT_800acae7 == '\0') && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    }
    else {
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
  FUN_8011c024(0,1);
  return;
}


