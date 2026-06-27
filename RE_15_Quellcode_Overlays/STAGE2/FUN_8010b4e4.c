/* FUN_8010b4e4 @ 0x8010b4e4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b4e4(void)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  if (*(char *)((int)_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x14;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 8;
  }
  func_0x8001aac4((int)(short)_DAT_800ac784[0x6f],(int)*(short *)((int)_DAT_800ac784 + 0x1be),0x10);
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
      uVar2 = func_0x8001af20();
      *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
           *(undefined2 *)((uVar2 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee8e84);
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      _DAT_800ac784[0x1e] = _DAT_80118bb0;
      func_0x8001af5c(0,0,*(ushort *)(_DAT_800ac784[0x1e] + 6) + 100,
                      *(ushort *)(_DAT_800ac784[0x1e] + 10) + 200,_DAT_800ac784 + 0x2c,0x808080);
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 1) != 0) {
        _DAT_800ac784[1] = 0x1000001;
        *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
        _DAT_800ac784[0xe] = (uint)*(byte *)((int)_DAT_800ac784 + 0x82) * -0x708;
        *(short *)((int)_DAT_800ac784 + 0x1ba) = (short)_DAT_800ac784[0xe];
      }
      iVar4 = 0xe1c;
      iVar5 = 0x15;
      do {
        iVar3 = iVar4 + _DAT_800ac784[0x62];
        iVar4 = iVar4 + -0xac;
        *(int *)(iVar3 + 0x2c) = (*(int *)(iVar3 + 0x2c) * 5) / 2;
        *(int *)(iVar3 + 0x30) = (*(int *)(iVar3 + 0x30) * 5) / 2;
        bVar1 = iVar5 != 0;
        *(int *)(iVar3 + 0x34) = (*(int *)(iVar3 + 0x34) * 5) / 2;
        iVar5 = iVar5 + -1;
      } while (bVar1);
      return;
    }
  }
  return;
}


