/* FUN_80115f70 @ 0x80115f70  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f70(int param_1)

{
  byte bVar1;
  short sVar2;
  int in_v0;
  uint uVar3;
  int in_v1;
  
  if (in_v1 == in_v0) {
    sVar2 = *(short *)(param_1 + 0x1d4);
    *(short *)(param_1 + 0x1d4) = sVar2 + -1;
    if ((sVar2 != 0) || (0x2f < (byte)_DAT_800ac784[0x74])) {
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
    if (1 < in_v1) {
      if (in_v1 == 2) {
        FUN_80116c68();
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
        return;
      }
      goto LAB_80116860;
    }
    if (in_v1 != 0) goto LAB_80116860;
    *(undefined1 *)(param_1 + 0x1d0) = 0x28;
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
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee0fcc);
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x800;
  *(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x166) =
         (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
  }
  _DAT_800ac784[0x1e] = *(uint *)((uint)*(byte *)((int)_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                  *(ushort *)(_DAT_800ac784[0x1e] + 6) + 200);
  uVar3 = _DAT_800ac784[0x62];
  *(uint *)(uVar3 + 0xc84) = uVar3 + 0xec;
  *(uint *)(uVar3 + 0xca8) = uVar3 + 0xac;
  *(undefined4 *)(uVar3 + 0xc44) = 0x66;
  *(undefined4 *)(uVar3 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(uVar3 + 0xc4c) = 0;
  *(undefined2 *)(uVar3 + 0xc78) = 0;
  *(undefined2 *)(uVar3 + 0xc7a) = 0;
  *(undefined2 *)(uVar3 + 0xc7c) = 0;
  func_0x80068098(uVar3 + 0xc78,uVar3 + 0xc30);
  return;
}


