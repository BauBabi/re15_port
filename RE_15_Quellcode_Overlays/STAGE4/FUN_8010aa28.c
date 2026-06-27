/* FUN_8010aa28 @ 0x8010aa28  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aa28(undefined4 param_1)

{
  uint *puVar1;
  undefined2 uVar2;
  uint uVar3;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_80118ccc)[(unaff_s0 & 0xff) * 8] * 0xac +
                               _DAT_800ac784[0x62] + 0x40);
      return;
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      puVar1 = _DAT_800ac784;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      *(undefined1 *)((int)puVar1 + 0x1d7) = 0x1f;
      func_0x8001aac4((int)(short)_DAT_800ac784[0x6f],(int)*(short *)((int)_DAT_800ac784 + 0x1be),
                      0x10);
      func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x100);
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010c03c(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                                  (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                                  (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                                  (_DAT_800aca90 - _DAT_800ac784[0xf]));
          *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
          (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee610c))();
          (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fee60cc))();
          *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
          *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
          func_0x80012974(4000);
          if ((char)_DAT_800ac784[0x78] != '\0') {
            *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
            func_0x80012a0c(5000);
          }
          if ((short)_DAT_800ac784[0x77] != 0) {
            *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
          }
          if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
            *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
            return;
          }
          *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
          FUN_8010c8ac();
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
  (**(code **)(&LAB_80119758 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}


