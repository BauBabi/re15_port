/* FUN_8011461c @ 0x8011461c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011461c(void)

{
  short *psVar1;
  short sVar2;
  undefined1 uVar3;
  char cVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint *puVar8;
  
  if ((*(char *)((int)_DAT_800ac784 + 7) == '\0') &&
     (func_0x80019700(0x82000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                      &DAT_80118f18), (short)_DAT_800ac784[0x76] != 0)) {
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
  }
  if (((*(char *)((int)_DAT_800ac784 + 7) == '\x01') ||
      (*(char *)((int)_DAT_800ac784 + 7) == '\x03')) &&
     ((*(byte *)((int)_DAT_800ac784 + 0x93) & 1) != 0)) {
    if ((*(byte *)((int)_DAT_800ac784 + 0x93) & 0x40) == 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 4;
      goto LAB_80114b3c;
    }
    puVar8 = (uint *)_DAT_800ac784[0x62];
    puVar8[0x1a] = 0x207f60;
    *(undefined2 *)(puVar8 + 0x25) = 0;
    *(undefined2 *)((int)puVar8 + 0x96) = 0xffce;
    *(undefined2 *)(puVar8 + 0x26) = 0;
    *(undefined2 *)((int)puVar8 + 0x9a) = 3;
    *(undefined2 *)(puVar8 + 0x27) = 0;
    *(undefined2 *)((int)puVar8 + 0x9e) = 0;
    *puVar8 = *puVar8 | 0x4a;
    uVar5 = _DAT_800ac784[0x62];
    *(undefined4 *)(uVar5 + 0xd2c) = 0x207f60;
    *(undefined2 *)(uVar5 + 0xd58) = 0;
    *(undefined2 *)(uVar5 + 0xd5a) = 0xffce;
    *(undefined2 *)(uVar5 + 0xd5c) = 0;
    *(undefined2 *)(uVar5 + 0xd5e) = 3;
    *(undefined2 *)(uVar5 + 0xd60) = 0;
    *(undefined2 *)(uVar5 + 0xd62) = 0;
    *(uint *)(uVar5 + 0xcc4) = *(uint *)(uVar5 + 0xcc4) | 0x4a;
    func_0x80019700(0x82000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x40,
                    &DAT_80118f18);
    func_0x80019700(0x82000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0xd04,
                    &DAT_80118f18);
    puVar8 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x78) = 9;
    sVar2 = (short)_DAT_800ac784[0x78];
    uVar5 = puVar8[0x62];
    *(short *)(_DAT_800ac784 + 0x78) = sVar2 + -1;
    while (sVar2 != 1) {
      iVar7 = (short)_DAT_800ac784[0x78] * 0x158 + uVar5;
      uVar6 = *(uint *)(iVar7 + 0xac);
      if ((uVar6 & 0x41) == 1) {
        *(uint *)(iVar7 + 0xac) = uVar6 | 0x1062;
        iVar7 = (short)_DAT_800ac784[0x78] * 0x158 + uVar5;
        *(uint *)(iVar7 + 0x158) = *(uint *)(iVar7 + 0x158) | 0x1062;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x121) = 0x14;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x12e) = 0xff80;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x122) = 8;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0xe4) = 0;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0xe6) = 0xff9c;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0xe8) = 0;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x1cd) = 0x14;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x1da) = 0xff80;
        *(undefined1 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x1ce) = 8;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 400) = 0;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x192) = 0xff9c;
        *(undefined2 *)((short)_DAT_800ac784[0x78] * 0x158 + uVar5 + 0x194) = 0;
      }
      sVar2 = (short)_DAT_800ac784[0x78];
      *(short *)(_DAT_800ac784 + 0x78) = sVar2 + -1;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
    uVar3 = 2;
code_r0x80114e00:
    *(undefined1 *)((int)_DAT_800ac784 + 7) = uVar3;
  }
  else {
LAB_80114b3c:
    puVar8 = _DAT_800ac784;
    switch(*(undefined1 *)((int)_DAT_800ac784 + 7)) {
    case 0:
      *(undefined2 *)(_DAT_800ac784 + 0x76) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 4;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 1;
      *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    case 1:
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      if (0xd < *(byte *)((int)_DAT_800ac784 + 0x95)) {
        *_DAT_800ac784 = *_DAT_800ac784 | 0x20000000;
      }
      cVar4 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + cVar4;
      break;
    case 2:
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 5;
      *(undefined1 *)((int)_DAT_800ac784 + 7) = 3;
      *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
    case 3:
      *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x20000000;
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      break;
    case 4:
      func_0x800453d0(7);
      _DAT_800ac784[1] = 0x3000007;
      break;
    case 5:
      psVar1 = (short *)((int)_DAT_800ac784 + 0xbe);
      *(short *)(_DAT_800ac784 + 0x2f) = (short)_DAT_800ac784[0x2f] + 8;
      *(short *)((int)puVar8 + 0xbe) = *psVar1 + 8;
      puVar8[0x31] = puVar8[0x31] & 0xff000000 | 0xffff38;
      puVar8[0x3b] = puVar8[0x3b] & 0xff000000 | 0xffff38;
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      uVar5 = _DAT_800ac784[0x27];
      *(short *)(_DAT_800ac784 + 0x27) = (short)uVar5 + -1;
      uVar3 = 6;
      if ((short)uVar5 != 1) {
        return;
      }
      goto code_r0x80114e00;
    }
  }
  return;
}


