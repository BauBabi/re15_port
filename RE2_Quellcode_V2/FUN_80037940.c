/* FUN_80037940 @ 0x80037940  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80037940(int param_1)

{
  char cVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  uint *puVar8;
  ushort uVar9;
  short local_30 [2];
  short local_2c;
  int local_28;
  int local_24;
  int local_20;
  
  uVar9 = *(short *)(param_1 + 0x76) + 0x200U & 0xfc00;
  iVar4 = (uint)*(byte *)(param_1 + 8) * 2;
  if (*(byte *)(param_1 + 8) < 0x10) {
    local_30[0] = *(short *)(&DAT_800a262c + iVar4);
  }
  else {
    local_30[0] = *(short *)(&DAT_800a258c + iVar4);
  }
  local_2c = 0;
  FUN_80077384((int)(short)uVar9,local_30,local_30);
  local_28 = (int)local_30[0] + *(int *)(param_1 + 0x38);
  local_24 = *(int *)(param_1 + 0x3c) + -0x708;
  local_20 = (int)local_2c + *(int *)(param_1 + 0x40);
  cVar1 = *(char *)(param_1 + 8);
  if (((cVar1 == '\x0f') || (cVar1 == 'E')) || (cVar1 == 'O')) {
    uVar7 = 0x800;
  }
  else {
    uVar7 = 0x1000;
  }
  uVar5 = FUN_8004c1bc(&local_28,0,1 << (*(byte *)(param_1 + 0x106) & 0x1f),uVar7);
  if ((((uVar5 & 2) == 0) || (0x1e < *(byte *)(param_1 + 0x106))) ||
     ((sVar3 = FUN_8004fba0(&local_28,*(undefined2 *)(param_1 + 0x9a),
                            (uint)*(byte *)(param_1 + 9) << 8,0), local_24 < sVar3 ||
      (((DAT_800dcbc8 & 1) != 0 || (iVar4 = FUN_800389e8(&local_28,param_1), iVar4 != 0)))))) {
    iVar4 = (uint)*(byte *)(param_1 + 8) * 2;
    if (*(byte *)(param_1 + 8) < 0x10) {
      local_30[0] = *(short *)(&DAT_800a266c + iVar4);
    }
    else {
      local_30[0] = *(short *)(&DAT_800a25cc + iVar4);
    }
    local_2c = 0;
    FUN_80077384((int)(short)uVar9,local_30,local_30);
    local_28 = (int)local_30[0] + *(int *)(param_1 + 0x38);
    local_20 = (int)local_2c + *(int *)(param_1 + 0x40);
    local_24 = *(undefined4 *)(param_1 + 0x3c);
    local_24 = FUN_8004fba0(&local_28,*(undefined2 *)(param_1 + 0x9a),
                            (uint)*(byte *)(param_1 + 9) << 8,0);
    cVar1 = *(char *)(param_1 + 8);
    if (((cVar1 == '\x0f') || (cVar1 == 'E')) || (cVar1 == 'O')) {
      uVar7 = 0x800;
    }
    else {
      uVar7 = 0x1000;
    }
    uVar5 = FUN_8004c1bc(&local_28,0,1 << (*(byte *)(param_1 + 0x106) & 0x1f),uVar7);
    if (local_24 <= *(int *)(param_1 + 0x3c)) {
      return 0;
    }
    if ((uVar5 & 4) == 0) {
      return 0;
    }
    puVar8 = &DAT_800d0324;
    if (&DAT_800d0324 < DAT_800d4224) {
      do {
        if ((((*puVar8 & 1) != 0) && ((*puVar8 & 2) == 0)) &&
           (puVar8 != *(uint **)(param_1 + 0x1e4))) {
          iVar6 = FUN_80038950(&local_28,puVar8,*(undefined2 *)(param_1 + 0x9a),1);
          iVar4 = local_24;
          if (iVar6 != 0) {
            return 0;
          }
          local_24 = *(undefined4 *)(param_1 + 0x3c);
          iVar6 = FUN_80038950(&local_28,puVar8,*(undefined2 *)(param_1 + 0x9a),1);
          if (iVar6 != 0) {
            return 0;
          }
          local_24 = (int)(short)iVar4;
        }
        puVar8 = puVar8 + 0x7e;
      } while (puVar8 < DAT_800d4224);
    }
    iVar4 = FUN_800389e8(&local_28,param_1);
    if (iVar4 != 0) {
      return 0;
    }
    if ((((DAT_800cfbd8 & 0x10000000) == 0) || (uVar2 = 8, param_1 != DAT_800cfe18)) &&
       (uVar2 = 0x19, *(char *)(param_1 + 5) == '\x04')) {
      uVar2 = 0x17;
    }
    *(undefined1 *)(param_1 + 5) = uVar2;
    *(undefined1 *)(param_1 + 6) = 2;
  }
  else {
    if ((((DAT_800cfbd8 & 0x10000000) == 0) || (uVar2 = 8, param_1 != DAT_800cfe18)) &&
       (uVar2 = 0x18, *(char *)(param_1 + 5) == '\x04')) {
      uVar2 = 0x16;
    }
    *(undefined1 *)(param_1 + 5) = uVar2;
    *(undefined1 *)(param_1 + 6) = 0;
  }
  *(undefined1 *)(param_1 + 7) = 0;
  *(undefined2 *)(param_1 + 0x214) = *(undefined2 *)(param_1 + 0x76);
  return 1;
}


