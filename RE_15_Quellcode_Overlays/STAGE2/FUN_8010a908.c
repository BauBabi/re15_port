/* FUN_8010a908 @ 0x8010a908  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010a908(undefined4 param_1)

{
  bool bVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_801178a8)[(unaff_s0 & 0xff) * 8] * 0xac +
                               _DAT_800ac784[0x62] + 0x40);
      return;
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      puVar2 = _DAT_800ac784;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      *(undefined1 *)((int)puVar2 + 0x1d7) = 0x1f;
      func_0x8001aac4((int)(short)_DAT_800ac784[0x6f],(int)*(short *)((int)_DAT_800ac784 + 0x1be),
                      0x10);
      func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010bf1c(0,0);
          }
        }
        else if (((ushort)_DAT_800ac784[0x76] & 0x40) == 0) {
          *(undefined2 *)(((ushort)_DAT_800ac784[0x76] & 0x40) + 0x1dc) = 0;
          *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x78) = 1;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1e1) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1e2) = 0;
          *(undefined1 *)((int)_DAT_800ac784 + 0x1e3) = 0;
          _DAT_800ac784[0xe] = 0xfffffb50;
          *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
          func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
          uVar3 = func_0x8001af20();
          *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
               *(undefined2 *)
                ((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
          *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
          _DAT_800ac784[0x1e] = _DAT_80118bb0;
          func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                          *(ushort *)(_DAT_800ac784[0x1e] + 10) + 200,_DAT_800ac784 + 0x2c,0x808080)
          ;
          if ((*(byte *)((int)_DAT_800ac784 + 9) & 1) != 0) {
            _DAT_800ac784[1] = 0x1000001;
            *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
            *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
            _DAT_800ac784[0xe] = (uint)*(byte *)((int)_DAT_800ac784 + 0x82) * -0x708;
            *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
          }
          iVar5 = 0xe1c;
          iVar6 = 0x15;
          do {
            iVar4 = iVar5 + _DAT_800ac784[0x62];
            iVar5 = iVar5 + -0xac;
            *(int *)(iVar4 + 0x2c) = (*(int *)(iVar4 + 0x2c) * 5) / 2;
            *(int *)(iVar4 + 0x30) = (*(int *)(iVar4 + 0x30) * 5) / 2;
            bVar1 = iVar6 != 0;
            *(int *)(iVar4 + 0x34) = (*(int *)(iVar4 + 0x34) * 5) / 2;
            iVar6 = iVar6 + -1;
          } while (bVar1);
          return;
        }
      }
      return;
    }
  }
  uVar3 = func_0x8001bc08(param_1);
  if ((uVar3 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar3 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
  }
  (**(code **)(&LAB_80118334 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}


