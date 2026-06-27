/* FUN_80113ff8 @ 0x80113ff8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113ff8(void)

{
  short *psVar1;
  short sVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint *puVar7;
  
  iVar4 = _DAT_800ac784;
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined2 *)(_DAT_800ac784 + 0x1d8) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118f18);
    puVar7 = *(uint **)(_DAT_800ac784 + 0x188);
    puVar7[0x1a] = 0x207f60;
    *(undefined2 *)(puVar7 + 0x25) = 0;
    *(undefined2 *)((int)puVar7 + 0x96) = 0xffce;
    *(undefined2 *)(puVar7 + 0x26) = 0;
    *(undefined2 *)((int)puVar7 + 0x9a) = 3;
    *(undefined2 *)(puVar7 + 0x27) = 0;
    *(undefined2 *)((int)puVar7 + 0x9e) = 0;
    *puVar7 = *puVar7 | 0x4a;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0xd2c) = 0x207f60;
    *(undefined2 *)(iVar4 + 0xd58) = 0;
    *(undefined2 *)(iVar4 + 0xd5a) = 0xffce;
    *(undefined2 *)(iVar4 + 0xd5c) = 0;
    *(undefined2 *)(iVar4 + 0xd5e) = 3;
    *(undefined2 *)(iVar4 + 0xd60) = 0;
    *(undefined2 *)(iVar4 + 0xd62) = 0;
    *(uint *)(iVar4 + 0xcc4) = *(uint *)(iVar4 + 0xcc4) | 0x4a;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118f18);
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xd04,&DAT_80118f18);
    func_0x800453d0(5);
    iVar4 = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x1e0) = 9;
    sVar2 = *(short *)(_DAT_800ac784 + 0x1e0);
    iVar4 = *(int *)(iVar4 + 0x188);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar2 + -1;
    while (sVar2 != 1) {
      iVar6 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4;
      uVar5 = *(uint *)(iVar6 + 0xac);
      if ((uVar5 & 0x41) == 1) {
        *(uint *)(iVar6 + 0xac) = uVar5 | 0x1062;
        iVar6 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4;
        *(uint *)(iVar6 + 0x158) = *(uint *)(iVar6 + 0x158) | 0x1062;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x121) = 0x14;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x12e) = 0xff80;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x122) = 8;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0xe4) = 0;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0xe6) = 0xff9c;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0xe8) = 0;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x1cd) = 0x14;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x1da) = 0xff80;
        *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x1ce) = 8;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 400) = 0;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x192) = 0xff9c;
        *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar4 + 0x194) = 0;
      }
      sVar2 = *(short *)(_DAT_800ac784 + 0x1e0);
      *(short *)(_DAT_800ac784 + 0x1e0) = sVar2 + -1;
    }
  case 1:
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    break;
  case 2:
    *(undefined4 *)(_DAT_800ac784 + 4) = 7;
    break;
  case 3:
    psVar1 = (short *)(_DAT_800ac784 + 0xbe);
    *(short *)(_DAT_800ac784 + 0xbc) = *(short *)(_DAT_800ac784 + 0xbc) + 8;
    *(short *)(iVar4 + 0xbe) = *psVar1 + 8;
    *(uint *)(iVar4 + 0xc4) = *(uint *)(iVar4 + 0xc4) & 0xff000000 | 0xffff38;
    *(uint *)(iVar4 + 0xec) = *(uint *)(iVar4 + 0xec) & 0xff000000 | 0xffff38;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 1) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    }
  }
  return;
}


