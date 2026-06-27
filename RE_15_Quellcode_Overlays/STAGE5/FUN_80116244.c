/* FUN_80116244 @ 0x80116244  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116244(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  short sVar6;
  uint local_28;
  undefined4 local_24;
  uint local_20;
  undefined4 local_1c;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar3 = (int)(((_DAT_800aca88 & 0xffff) - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >>
            0x10;
    iVar4 = (int)(((_DAT_800aca90 & 0xffff) - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >>
            0x10;
    uVar5 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar5;
    func_0x8001bd60(0xfffffff6,0x14);
    local_28 = _DAT_800aca88;
    local_24 = _DAT_800aca8c;
    local_20 = _DAT_800aca90;
    local_1c = _DAT_800aca94;
    if ((*(byte *)(_DAT_800ac784 + 0x1dd) & 1) != 0) {
      local_28 = (uint)(short)*(ushort *)(_DAT_800ac784 + 0x1d4);
      local_20 = (uint)(short)*(ushort *)(_DAT_800ac784 + 0x1d6);
      iVar3 = (int)(((uint)*(ushort *)(_DAT_800ac784 + 0x1d4) -
                    (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10;
      iVar4 = (int)(((uint)*(ushort *)(_DAT_800ac784 + 0x1d6) -
                    (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10;
      uVar5 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
      *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar5;
    }
    func_0x80039e7c(&local_28,0,0);
    if (*(char *)(_DAT_800ac784 + 0x1de) != '\0') {
      *(char *)(_DAT_800ac784 + 0x1de) = *(char *)(_DAT_800ac784 + 0x1de) + -1;
      if (*(char *)(_DAT_800ac784 + 0x1de) == '\0') {
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      }
    }
    if (*(char *)(_DAT_800ac784 + 0x1df) != '\0') {
      *(char *)(_DAT_800ac784 + 0x1df) = *(char *)(_DAT_800ac784 + 0x1df) + -1;
    }
    (**(code **)(&LAB_8011fe48 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
    func_0x8001b4e4();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) & 0xfd;
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x8001b38c();
    if (((*(byte *)(_DAT_800ac784 + 9) & 0x10) == 0) && (*(short *)(_DAT_800ac784 + 0x9a) < 200)) {
      bVar1 = *(byte *)(_DAT_800ac784 + 0x1dd);
      *(byte *)(_DAT_800ac784 + 0x1dd) = bVar1 ^ 4;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar6 = -(ushort)((bVar1 & 4) != 0);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x736) = sVar6 * sVar2 * -3 + 0x1000;
      func_0x8003a95c(iVar3 + 0x6b8,1);
      *(undefined2 *)(iVar3 + 0x736) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x7e2) = sVar6 * sVar2 * 3 + 0x1000;
      func_0x8003a95c(iVar3 + 0x764,1);
      *(undefined2 *)(iVar3 + 0x7e2) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x88e) = sVar6 * sVar2 * -2 + 0x1000;
      func_0x8003a95c(iVar3 + 0x810,1);
      *(undefined2 *)(iVar3 + 0x88e) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x532) = sVar6 * sVar2 * 3 + 0x1000;
      func_0x8003a95c(iVar3 + 0x4b4,1);
      *(undefined2 *)(iVar3 + 0x532) = 0xfff;
      iVar3 = *(int *)(_DAT_800ac784 + 0x188);
      sVar2 = func_0x8001af20();
      *(short *)(iVar3 + 0x5de) = sVar6 * sVar2 * -2 + 0x1000;
      func_0x8003a95c(iVar3 + 0x560,1);
      *(undefined2 *)(iVar3 + 0x5de) = 0xfff;
      if (*(short *)(_DAT_800ac784 + 0x9a) < 100) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
      }
    }
    *(undefined4 *)(_DAT_800ac784 + 0x1d8) = *(undefined4 *)(_DAT_800ac784 + 4);
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


