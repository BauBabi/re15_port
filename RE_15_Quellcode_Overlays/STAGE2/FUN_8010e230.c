/* FUN_8010e230 @ 0x8010e230  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e230(void)

{
  byte bVar1;
  int in_at;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  uVar3 = (uint)(bVar1 < 2);
  if (bVar1 != 1) {
    if (uVar3 == 0) {
      uVar3 = 2;
      if (bVar1 == 2) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
        *(undefined4 *)(_DAT_800ac784 + 4) = 7;
        return;
      }
      goto LAB_8010ec1c;
    }
    if (bVar1 != 0) goto LAB_8010ec1c;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  if ((*(char *)(_DAT_800ac784 + 0x1e0) == '\0') && (*(char *)(_DAT_800ac784 + 0x95) == '\x14')) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x15) {
    return;
  }
  uVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffb0,
                          (int)*(short *)(_DAT_800ac784 + 0x1ba));
LAB_8010ec1c:
  (**(code **)(in_at + -0x7270 + uVar3))();
  return;
}


