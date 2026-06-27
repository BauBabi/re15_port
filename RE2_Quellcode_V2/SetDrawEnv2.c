/* SetDrawEnv2 @ 0x8009151c  (Ghidra headless, raw MIPS overlay) */

void SetDrawEnv2(int param_1,ushort *param_2)

{
  ushort uVar1;
  ushort uVar2;
  short sVar3;
  undefined4 uVar4;
  int iVar5;
  char cVar6;
  undefined4 local_30;
  undefined4 local_2c;
  
  uVar4 = get_cs((int)(short)*param_2,(int)(short)param_2[1]);
  *(undefined4 *)(param_1 + 4) = uVar4;
  uVar4 = get_ce((int)(((uint)param_2[2] + (uint)*param_2 + -1) * 0x10000) >> 0x10,
                 (int)(((uint)param_2[1] + (uint)param_2[3] + -1) * 0x10000) >> 0x10);
  *(undefined4 *)(param_1 + 8) = uVar4;
  uVar4 = get_ofs((int)(short)param_2[4],(int)(short)param_2[5]);
  *(undefined4 *)(param_1 + 0xc) = uVar4;
  uVar4 = get_mode(*(undefined1 *)((int)param_2 + 0x17),(char)param_2[0xb],param_2[10]);
  *(undefined4 *)(param_1 + 0x10) = uVar4;
  uVar4 = get_tw(param_2 + 6);
  *(undefined4 *)(param_1 + 0x14) = uVar4;
  *(undefined4 *)(param_1 + 0x18) = 0xe6000000;
  cVar6 = '\a';
  if ((char)param_2[0xc] != '\0') {
    uVar4 = *(undefined4 *)param_2;
    uVar1 = param_2[2];
    uVar2 = 0;
    if ((-1 < (short)uVar1) && (uVar2 = DAT_800b2704 - 1, (int)(short)uVar1 <= DAT_800b2704 + -1)) {
      uVar2 = uVar1;
    }
    iVar5 = (int)(short)param_2[3];
    if (iVar5 < 0) {
      sVar3 = 0;
    }
    else {
      sVar3 = DAT_800b2706 + -1;
      if (iVar5 <= DAT_800b2706 + -1) {
        SYS_OBJ_1790(iVar5);
        return;
      }
    }
    local_2c = CONCAT22(sVar3,uVar2);
    if (((*param_2 & 0x3f) != 0) || ((uVar2 & 0x3f) != 0)) {
      local_30 = CONCAT22(param_2[1] - param_2[5],*param_2 - param_2[4]);
      *(uint *)(param_1 + 0x1c) =
           (uint)*(byte *)((int)param_2 + 0x1b) << 0x10 | (uint)(byte)param_2[0xd] << 8 | 0x60000000
           | (uint)*(byte *)((int)param_2 + 0x19);
      *(undefined4 *)(param_1 + 0x20) = local_30;
      *(undefined4 *)(param_1 + 0x24) = local_2c;
      SYS_OBJ_18AC();
      return;
    }
    cVar6 = '\n';
    *(uint *)(param_1 + 0x1c) =
         (uint)*(byte *)((int)param_2 + 0x1b) << 0x10 | (uint)(byte)param_2[0xd] << 8 | 0x2000000 |
         (uint)*(byte *)((int)param_2 + 0x19);
    *(undefined4 *)(param_1 + 0x20) = uVar4;
    *(undefined4 *)(param_1 + 0x24) = local_2c;
  }
  *(char *)(param_1 + 3) = cVar6 + -1;
  return;
}


