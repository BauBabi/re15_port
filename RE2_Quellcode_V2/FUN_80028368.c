/* FUN_80028368 @ 0x80028368  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80028368(int param_1,undefined4 param_2,ushort *param_3,undefined4 param_4)

{
  byte bVar1;
  ushort uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  byte *pbVar6;
  char cVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  uint uVar11;
  int iVar12;
  uint *puVar13;
  uint *puVar14;
  uint local_48;
  ushort *local_30;
  
  local_30 = param_3 + 4;
  cVar7 = '\0';
  uVar11 = (uint)*(byte *)(param_1 + 0x107);
  puVar9 = *(undefined4 **)(param_1 + 0x198);
  iVar12 = *(int *)(param_1 + 0x14);
  local_48 = (uint)param_3[2];
  puVar13 = (uint *)((int)param_3 + (*param_3 & 0xfffc));
  puVar9[0x1d] = param_1 + 0x24;
  puVar9[0x25] = param_1;
  *(undefined4 *)(param_1 + 0x13c) = 0;
  *(undefined2 *)(param_1 + 0x140) = 0;
  uVar3 = DAT_8009db4c;
  uVar4 = DAT_8009db48;
  *(undefined4 *)(param_1 + 0x11c) = DAT_8009db44;
  *(undefined4 *)(param_1 + 0x120) = uVar4;
  *(undefined4 *)(param_1 + 0x124) = uVar3;
  uVar3 = DAT_8009db58;
  uVar4 = DAT_8009db54;
  *(undefined4 *)(param_1 + 0x128) = DAT_8009db50;
  *(undefined4 *)(param_1 + 300) = uVar4;
  *(undefined4 *)(param_1 + 0x130) = uVar3;
  uVar4 = DAT_8009db60;
  *(undefined4 *)(param_1 + 0x134) = DAT_8009db5c;
  *(undefined4 *)(param_1 + 0x138) = uVar4;
  *(int *)(param_1 + 0x80) = param_1 + 0x11c;
  if (uVar11 != 0) {
    puVar8 = puVar9 + 0x25;
    param_3 = param_3 + 6;
    puVar10 = puVar9;
    puVar14 = puVar13;
    do {
      uVar11 = uVar11 - 1;
      puVar8[-9] = 0x808080;
      puVar8[-0x13] = 0x1000;
      puVar8[-0x12] = 0;
      puVar8[-0x11] = 0x1000;
      puVar8[-0x10] = 0;
      puVar8[-0xf] = 0x1000;
      puVar8[-0xe] = 0;
      puVar8[-0xd] = 0;
      puVar8[-0xc] = 0;
      puVar8[-0x1f] = 0x1000;
      puVar8[-0x1e] = 0;
      puVar8[-0x1d] = 0x1000;
      puVar8[-0x1c] = 0;
      puVar8[-0x1b] = 0x1000;
      puVar8[-0x24] = 0;
      puVar8[-0xb] = 0;
      *(undefined2 *)(puVar8 + -10) = 0;
      *(undefined2 *)(puVar8 + 1) = 0;
      *(undefined2 *)((int)puVar8 + 6) = 0;
      *(undefined2 *)(puVar8 + 2) = 0;
      *(undefined2 *)((int)puVar8 + 10) = 0;
      *(undefined2 *)(puVar8 + 3) = 0;
      *(undefined2 *)((int)puVar8 + 0xe) = 0;
      *puVar10 = 1;
      *(undefined2 *)(puVar8 + -0xe) = 0;
      puVar8[-0x23] = iVar12;
      puVar8[-0x21] = iVar12 + 0x1c;
      *(char *)(puVar8 + -7) = cVar7;
      uVar4 = FUN_8002cbc4(puVar10 + 1,param_2,param_4);
      param_2 = FUN_8002cd24(puVar10 + 1,uVar4,param_4);
      uVar2 = *local_30;
      puVar8[-0x1a] = (int)(short)uVar2;
      *(ushort *)(puVar8 + -0x17) = uVar2;
      uVar2 = param_3[-1];
      local_30 = local_30 + 3;
      puVar8[-0x19] = (int)(short)uVar2;
      *(ushort *)((int)puVar8 + -0x5a) = uVar2;
      uVar2 = *param_3;
      puVar8[-0x18] = (int)(short)uVar2;
      *(ushort *)(puVar8 + -0x16) = uVar2;
      param_3 = param_3 + 3;
      if (local_48 == 0) {
        puVar8[-8] = &DAT_8009db44;
        *puVar8 = 0;
      }
      else {
        pbVar6 = (byte *)((int)puVar13 + (*puVar14 >> 0x10));
        uVar5 = *puVar14 & 0xffff;
        if (uVar5 != 0) {
          do {
            uVar5 = uVar5 - 1;
            puVar9[(uint)*pbVar6 * 0x2b + 0x1d] = puVar10 + 0x12;
            bVar1 = *pbVar6;
            pbVar6 = pbVar6 + 1;
            puVar9[(uint)bVar1 * 0x2b + 0x25] = puVar10;
          } while (uVar5 != 0);
        }
        puVar14 = puVar14 + 1;
        local_48 = local_48 - 1;
      }
      puVar8 = puVar8 + 0x2b;
      puVar10 = puVar10 + 0x2b;
      iVar12 = iVar12 + 0x38;
      cVar7 = cVar7 + '\x01';
    } while (uVar11 != 0);
  }
  return param_2;
}


