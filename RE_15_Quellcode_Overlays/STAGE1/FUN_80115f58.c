/* FUN_80115f58 @ 0x80115f58  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f58(void)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    uVar2 = _DAT_800ac784[0x75];
    *(short *)(_DAT_800ac784 + 0x75) = (short)uVar2 + -1;
    if (((short)uVar2 != 0) || (0x2f < (byte)_DAT_800ac784[0x74])) {
      return;
    }
    if ((byte)_DAT_800ac784[0x74] < 8) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
      goto LAB_80116860;
    }
    FUN_80116bec();
    *(char *)(_DAT_800ac784 + 0x74) = (char)_DAT_800ac784[0x74] + '\x01';
    bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  }
  else {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        FUN_80116c68();
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
        return;
      }
      goto LAB_80116860;
    }
    if (bVar1 != 0) goto LAB_80116860;
    *(undefined1 *)(_DAT_800ac784 + 0x74) = 0x28;
    FUN_80116d00();
    bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  }
  *(undefined2 *)(bVar1 + 0x1dd) = 0;
LAB_80116860:
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 4;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
  uVar2 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee0fcc);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x166) =
         (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
  }
  _DAT_800ac784[0x1e] = *(uint *)((uint)*(byte *)((int)_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 6) + 200,_DAT_800ac784 + 0x2c,0x808080);
  uVar2 = _DAT_800ac784[0x62];
  *(uint *)(uVar2 + 0xc84) = uVar2 + 0xec;
  *(uint *)(uVar2 + 0xca8) = uVar2 + 0xac;
  *(undefined4 *)(uVar2 + 0xc44) = 0x66;
  *(undefined4 *)(uVar2 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(uVar2 + 0xc4c) = 0;
  *(undefined2 *)(uVar2 + 0xc78) = 0;
  *(undefined2 *)(uVar2 + 0xc7a) = 0;
  *(undefined2 *)(uVar2 + 0xc7c) = 0;
  func_0x80068098(uVar2 + 0xc78,uVar2 + 0xc30);
  return;
}


