/* FUN_8010ab60 @ 0x8010ab60  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ab60(undefined4 param_1)

{
  uint *puVar1;
  short sVar2;
  uint uVar3;
  uint unaff_s0;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    param_1 = 8;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011d8f4)[(unaff_s0 & 0xff) * 8] * 0xac +
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
      puVar1 = _DAT_800ac784;
      if (*(char *)((int)_DAT_800ac784 + 0x8f) == '\0') {
        if ((*(uint *)_DAT_800ac784[0x5a] & 0x2000) == 0) {
          if ((_DAT_800ac784[0x76] & 0x20) == 0) {
            FUN_8010c174(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
            *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
            return;
          }
          sVar2 = (short)_DAT_800ac784[0x77] + 1;
          *(short *)(_DAT_800ac784 + 0x77) = sVar2;
          *(char *)((int)puVar1 + 5) = (char)sVar2;
          *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
          if ((DAT_800acae7 != '\0') &&
             (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
            *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
            uVar3 = func_0x8001af20(0,1);
            if ((uVar3 & 1) != 0) {
              *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
              FUN_8010d1a4();
              return;
            }
          }
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
  (**(code **)(&LAB_8011e380 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}


