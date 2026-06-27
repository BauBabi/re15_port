/* FUN_80031070 @ 0x80031070  (Ghidra headless, raw MIPS overlay) */

void FUN_80031070(uint param_1,int param_2,uint param_3,undefined1 param_4)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  undefined *puVar4;
  uint uVar5;
  int iVar6;
  ushort *puVar7;
  undefined *puVar8;
  undefined *p;
  undefined *puVar9;
  ushort local_48;
  
  p = DAT_800e8430;
  iVar6 = (param_3 & 7) << 1;
  uVar5 = (uint)DAT_800ce5e0;
  if ((param_3 & 7) == 0) {
    iVar6 = 6;
  }
  puVar9 = &DAT_800e2ab0 + uVar5 * 0xc0 + iVar6 * 0xc;
  local_48 = (ushort)((param_3 & 0x30) << 3) | 0x7810;
  pbVar1 = (byte *)FUN_80030b9c(param_4);
  bVar2 = *pbVar1;
  if (bVar2 != 0xf7) {
    puVar7 = (ushort *)(p + 0xe);
    do {
      iVar6 = 0xe;
      puVar4 = puVar9;
      if (bVar2 == 0xee) {
        pbVar1 = pbVar1 + 1;
        *(byte *)((int)puVar7 + -1) = (*pbVar1 / 0x12) * '\x0e' + -0x2e;
LAB_800312c4:
        puVar7[-0xffffffff00000005] = 0x8080;
        puVar7[-0xffffffff00000004] = 0x6480;
        puVar7[1] = 0xe;
        puVar7[2] = 0xe;
        *(uint *)(puVar7 + -3) = param_1 & 0xffff | param_2 << 0x10;
        bVar2 = *pbVar1;
        puVar8 = p + 0x14;
        *puVar7 = local_48;
        *(byte *)(puVar7 + -1) = (bVar2 % 0x12) * '\x0e';
        AddPrim(puVar4,p);
        puVar7 = puVar7 + 10;
        p = puVar8;
        if (puVar8 == &UNK_800e4030 + uVar5 * 0x1400) {
          DAT_800e8430 = puVar8;
          return;
        }
      }
      else {
        if (0xee < bVar2) {
          if (bVar2 == 0xef) {
            pbVar1 = pbVar1 + 1;
            *(byte *)((int)puVar7 + -1) = (*pbVar1 / 0x12) * '\x0e';
            puVar4 = puVar9 + 0xc;
          }
          else {
            if (bVar2 != 0xf0) goto LAB_8003125c;
            pbVar1 = pbVar1 + 1;
            *(byte *)((int)puVar7 + -1) = (*pbVar1 / 0x12) * '\x0e' + -0x3c;
            puVar4 = puVar9 + 0xc;
          }
          goto LAB_800312c4;
        }
        if (bVar2 != 0) {
LAB_8003125c:
          *(byte *)((int)puVar7 + -1) = (*pbVar1 / 0x12) * '\x0e' + '\x1c';
          if (((param_3 & 0x4000) != 0) && (iVar3 = (uint)*pbVar1 * 2, *pbVar1 < 0x57)) {
            param_1 = param_1 - (int)(&DAT_8009de34)[iVar3];
            iVar6 = (int)(&DAT_8009de35)[iVar3] + (int)(&DAT_8009de34)[iVar3];
          }
          goto LAB_800312c4;
        }
        if ((param_3 & 0x4000) != 0) {
          param_1 = param_1 - (int)DAT_8009de34;
          iVar6 = (int)DAT_8009de35 + (int)DAT_8009de34;
        }
      }
      pbVar1 = pbVar1 + 1;
      bVar2 = *pbVar1;
      param_1 = param_1 + iVar6;
    } while (bVar2 != 0xf7);
  }
  DAT_800e8430 = p;
  return;
}


