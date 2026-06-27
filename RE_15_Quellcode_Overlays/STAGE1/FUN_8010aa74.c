/* FUN_8010aa74 @ 0x8010aa74  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010aa74(void)

{
  uint *puVar1;
  uint uVar2;
  uint unaff_s0;
  int iVar3;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar3 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_1)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_2)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&LAB_8011f7a4_3)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a8)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7a9)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7aa)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011f7ab)[iVar3] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
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
            FUN_8010c088(0,0);
          }
        }
        else if ((_DAT_800ac784[0x76] & 0x40) == 0) {
          FUN_8010bf68(0,1);
          return;
        }
      }
      return;
    }
  }
  uVar2 = func_0x8001bc08();
  if ((uVar2 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar2 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
  }
  (**(code **)(&DAT_80120230 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}


