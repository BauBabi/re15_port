/* FUN_8011b39c @ 0x8011b39c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b39c(void)

{
  byte bVar1;
  char cVar2;
  byte bVar3;
  ushort uVar4;
  int iVar5;
  int unaff_s0;
  int local_20;
  undefined4 local_1c;
  int local_18;
  undefined4 local_14;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  bVar3 = bVar1 < 2;
  if (bVar1 != 1) {
    if (!(bool)bVar3) {
      bVar3 = 0;
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_8011bddc;
    }
    if (bVar1 != 0) goto LAB_8011bddc;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x1f) + 0x50;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    iVar5 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(int *)(iVar5 + -0x7fedec78);
    local_1c = *(undefined4 *)(iVar5 + -0x7fedec74);
    local_18 = *(int *)(iVar5 + -0x7fedec70);
    local_14 = *(undefined4 *)(iVar5 + -0x7fedec6c);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(0);
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) == 0) {
    return;
  }
  bVar3 = func_0x800245d8(0);
LAB_8011bddc:
  if ((bVar3 & 0x20) == 0) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_20 = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18 = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_1c = 0;
    func_0x80039e7c(&local_20,0,0);
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fedea68))();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(unaff_s0 + 0x14,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


