/* FUN_80113aa4 @ 0x80113aa4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113aa4(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  ushort uVar4;
  int iVar5;
  uint *puVar6;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 4) = 1;
      *(byte *)(_DAT_800ac784 + 6) = (*(byte *)(_DAT_800ac784 + 9) & 1) + 3;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
      *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 10;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&DAT_80118f18);
    func_0x800453d0(2);
    if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x40) != 0) {
      uVar4 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x1e0) = uVar4 & 7;
      *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 8;
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
      sVar2 = *(short *)(_DAT_800ac784 + 0x1e4);
      *(short *)(_DAT_800ac784 + 0x1e4) = sVar2 + -1;
      while (sVar2 != 0) {
        if ((&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] ==
            (&DAT_80118f30)[*(short *)(_DAT_800ac784 + 0x1e4)]) {
          *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 1;
        }
        sVar2 = *(short *)(_DAT_800ac784 + 0x1e4);
        *(short *)(_DAT_800ac784 + 0x1e4) = sVar2 + -1;
      }
      if (*(short *)(_DAT_800ac784 + 0x1e2) == 0) {
        (&DAT_80118f30)[*(short *)(_DAT_800ac784 + 0x1de)] =
             (&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)];
        puVar6 = (uint *)((uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                         *(int *)(_DAT_800ac784 + 0x188));
        puVar6[0x1a] = 0x207f60;
        *(undefined2 *)(puVar6 + 0x25) = 0;
        *(undefined2 *)((int)puVar6 + 0x96) = 0xffce;
        *(undefined2 *)(puVar6 + 0x26) = 0;
        *(undefined2 *)((int)puVar6 + 0x9a) = 3;
        *(undefined2 *)(puVar6 + 0x27) = 0;
        *(undefined2 *)((int)puVar6 + 0x9e) = 0;
        *puVar6 = *puVar6 | 0x4a;
        iVar5 = (uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                *(int *)(_DAT_800ac784 + 0x188);
        *(undefined4 *)(iVar5 + 0x114) = 0x207f60;
        *(undefined2 *)(iVar5 + 0x140) = 0;
        *(undefined2 *)(iVar5 + 0x142) = 0xffce;
        *(undefined2 *)(iVar5 + 0x144) = 0;
        *(undefined2 *)(iVar5 + 0x146) = 3;
        *(undefined2 *)(iVar5 + 0x148) = 0;
        *(undefined2 *)(iVar5 + 0x14a) = 0;
        *(uint *)(iVar5 + 0xac) = *(uint *)(iVar5 + 0xac) | 0x4a;
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                        0x198,&DAT_80118f18);
        func_0x80019700(0x82000,0,
                        *(int *)(_DAT_800ac784 + 0x188) +
                        (uint)(byte)(&DAT_80118f28)[*(short *)(_DAT_800ac784 + 0x1e0)] * 0xac +
                        0x198,&DAT_80118f18);
        func_0x800453d0(5);
        *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
      }
    }
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  return;
}


