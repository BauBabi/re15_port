/* DsPacket @ 0x8008a5a8  (Ghidra headless, raw MIPS overlay) */

int DsPacket(u_char mode,DslLOC *pos,u_char com,DslCB func,int count)

{
  uint uVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined1 *puVar7;
  int iVar8;
  int *piVar9;
  undefined1 local_88 [8];
  int local_80 [2];
  undefined1 local_78;
  u_char local_77 [7];
  u_char *local_70;
  undefined1 local_68;
  undefined1 auStack_67 [7];
  undefined1 *local_60;
  u_char local_58;
  DslCB local_4c;
  
  iVar8 = 0;
  puVar7 = local_88;
  do {
    *puVar7 = 0;
    *(undefined4 *)(puVar7 + 8) = 0;
    *(undefined4 *)(puVar7 + 0xc) = 0;
    iVar8 = iVar8 + 1;
    puVar7 = puVar7 + 0x10;
  } while (iVar8 < 5);
  local_88[0] = 9;
  local_78 = 0xe;
  local_70 = local_77;
  local_77[0] = mode;
  iVar8 = DsPosToInt(pos);
  if (-1 < iVar8) {
    local_68 = 2;
    uVar1 = (uint)&pos->track & 3;
    uVar3 = (uint)pos & 3;
    uVar3 = (*(int *)(&pos->track + -uVar1) << (3 - uVar1) * 8 | 0xffffffffU >> (uVar1 + 1) * 8 & 2)
            & -1 << (4 - uVar3) * 8 | *(uint *)((int)pos - uVar3) >> uVar3 * 8;
    auStack_67._3_4_ = auStack_67._3_4_ & 0xffffff00 | uVar3 >> 0x18;
    uVar1 = (uint)auStack_67 & 3;
    puVar2 = (uint *)(auStack_67 + -uVar1);
    *puVar2 = *puVar2 & 0xffffffffU >> (4 - uVar1) * 8 | uVar3 << uVar1 * 8;
    local_60 = auStack_67;
    local_58 = com;
    local_4c = func;
    switch(com) {
    case '\x03':
    case '\x06':
    case '\x1b':
      puVar7 = local_88;
      iVar8 = 0;
      if (DAT_800c3e80 + 4 < 9) {
        iVar4 = DAT_800ad22c + 1;
        piVar9 = local_80 + 1;
        DAT_800ad22c = iVar4;
        do {
          piVar5 = (int *)CQ_last_queue();
          if (piVar5 == (int *)0x0) {
            return 0;
          }
          *piVar5 = iVar4;
          *(undefined1 *)(piVar5 + 1) = *puVar7;
          if (piVar9[-1] != 0) {
            parcpy((int)piVar5 + 5,puVar7 + 1);
            piVar5[3] = (int)piVar5 + 5;
            iVar8 = DSSYS_2_OBJ_D54();
            return iVar8;
          }
          piVar5[3] = 0;
          iVar6 = *piVar9;
          piVar9 = piVar9 + 4;
          piVar5[5] = count;
          piVar5[4] = iVar6;
          iVar8 = iVar8 + 1;
          DAT_800c3e80 = DAT_800c3e80 + 1;
          puVar7 = puVar7 + 0x10;
        } while (iVar8 < 4);
        iVar8 = DS_system_status(0);
        if (iVar8 != 1) {
          return iVar4;
        }
        if ((&DAT_800c3db8)[DAT_800c3e7c * 6] != iVar4) {
          return iVar4;
        }
        CQ_execute();
        iVar8 = DSSYS_2_OBJ_DE0();
        return iVar8;
      }
      break;
    case '\x15':
    case '\x16':
      puVar7 = local_88;
      iVar8 = 0;
      if (DAT_800c3e80 + 4 < 9) {
        iVar4 = DAT_800ad22c + 1;
        piVar9 = local_80 + 1;
        DAT_800ad22c = iVar4;
        while( true ) {
          piVar5 = (int *)CQ_last_queue();
          if (piVar5 == (int *)0x0) {
            return 0;
          }
          *piVar5 = iVar4;
          *(undefined1 *)(piVar5 + 1) = *puVar7;
          if (piVar9[-1] != 0) break;
          piVar5[3] = 0;
          iVar6 = *piVar9;
          piVar9 = piVar9 + 4;
          piVar5[5] = count;
          piVar5[4] = iVar6;
          iVar8 = iVar8 + 1;
          DAT_800c3e80 = DAT_800c3e80 + 1;
          puVar7 = puVar7 + 0x10;
          if (3 < iVar8) {
            iVar8 = DSSYS_2_OBJ_D88();
            return iVar8;
          }
        }
        parcpy((int)piVar5 + 5,puVar7 + 1);
        piVar5[3] = (int)piVar5 + 5;
        iVar8 = DSSYS_2_OBJ_C74();
        return iVar8;
      }
    }
  }
  return 0;
}


