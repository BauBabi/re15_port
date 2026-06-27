/* FUN_80111ed4 @ 0x80111ed4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111ed4(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        *(undefined2 *)(_DAT_800ac784 + 4) = 1;
        *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
        *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xf;
        return;
      }
      goto code_r0x801127e4;
    }
    if (uVar2 != 0) goto code_r0x801127e4;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    func_0x80019700(0xd002000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118ee8);
    func_0x800453d0(4);
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar1;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar2;
code_r0x801127e4:
  *(char *)(uVar2 + 6) = (char)((*(ushort *)(uVar2 + 0x1d0) & 1) << 2);
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
  return;
}


