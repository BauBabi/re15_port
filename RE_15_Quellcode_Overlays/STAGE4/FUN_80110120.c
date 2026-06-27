/* FUN_80110120 @ 0x80110120  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110120(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int extraout_v1;
  int unaff_s0;
  int iVar4;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int iStack_c;
  int iStack_4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  uVar3 = (uint)(bVar1 < 2);
  if (bVar1 != 1) {
    if (uVar3 == 0) {
      uVar3 = 2;
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x244,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_80110b44;
    }
    if (bVar1 != 0) goto LAB_80110b44;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    iVar4 = (*(byte *)(_DAT_800ac784 + 6) & 1) * 0x20;
    local_20 = *(undefined4 *)(iVar4 + -0x7fee616c);
    local_1c = *(undefined4 *)(iVar4 + -0x7fee6168);
    local_18 = *(undefined4 *)(iVar4 + -0x7fee6164);
    local_14 = *(undefined4 *)(iVar4 + -0x7fee6160);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_20);
    func_0x800453d0(7);
    if (_DAT_800b0fe0 < 3) {
      *(int *)(*(byte *)(_DAT_800ac784 + 0x1c6) + 0x3c) =
           *(int *)(*(byte *)(_DAT_800ac784 + 0x1c6) + 0x3c) - (extraout_v1 + 0x7ff4efc8);
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if (*(char *)(_DAT_800ac784 + 0x95) != '\x13') {
    return;
  }
  uVar3 = func_0x800453d0(1);
LAB_80110b44:
  iVar4 = unaff_s0 + uVar3;
  func_0x80022da0(&local_20,iVar4 + -0x140,&local_20);
  func_0x80022da0(&local_20,iVar4 + -0x94,&local_20);
  func_0x80022da0(&local_20,iVar4 + 0x18,&local_20);
  *(int *)(_DAT_800ac784 + 0x34) =
       *(int *)(_DAT_800ac784 + 0x34) - (iStack_c - *(int *)(iVar4 + 0x54));
  *(int *)(_DAT_800ac784 + 0x3c) =
       *(int *)(_DAT_800ac784 + 0x3c) - (iStack_4 - *(int *)(iVar4 + 0x5c));
  return;
}


