void FUN_80028868(void)

{
  byte bVar1;
  int iVar2;
  char cVar3;
  uint uVar4;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  uint *puVar8;
  uint *puVar9;
  uint *puVar10;
  uint uVar11;
  ushort uVar12;
  byte *unaff_s6;
  
  uVar4 = (uint)DAT_800aca34;
  uVar11 = CONCAT22(DAT_800b8536,DAT_800b8534);
  uVar12 = 0x7810;
  if ((DAT_800b8538 & 0xf) == 0) {
    iVar2 = 6;
  }
  else {
    iVar2 = (DAT_800b8538 & 7) << 1;
  }
  puVar10 = DAT_800a7394;
  if (DAT_800b8528 != DAT_800b852c) {
    puVar9 = DAT_800a7394 + 4;
    pbVar6 = DAT_800b8528;
    do {
      bVar1 = DAT_800c4416;
      if (*pbVar6 == 0) {
LAB_80028bc8:
        uVar11 = uVar11 + bVar1;
        pbVar7 = pbVar6 + 1;
      }
      else {
        switch(*pbVar6) {
        case 2:
        case 4:
          pbVar6 = pbVar6 + 1;
        case 1:
          pbVar7 = pbVar6 + 1;
          break;
        default:
          bVar1 = *pbVar6;
          *(byte *)(puVar9 + -1) = bVar1 << 4;
          *(byte *)((int)puVar9 + -3) = (bVar1 & 0xf0) + 0x20;
          puVar8 = (uint *)(&DAT_800b829c + (uint)DAT_800aca34 * 0xc0 + iVar2 * 0xc);
          puVar9[-2] = uVar11;
          puVar9[-3] = 0x64808080;
          *(ushort *)((int)puVar9 + -2) = uVar12;
          *puVar9 = 0x100010;
          bVar1 = (&DAT_800c4416)[*pbVar6];
          puVar9 = puVar9 + 5;
                    /* Probable PsyQ macro: addPrim(). */
          uVar5 = (uint)puVar10 & 0xffffff;
          *puVar10 = *puVar10 & 0xff000000 | *puVar8 & 0xffffff;
          puVar10 = puVar10 + 5;
          *puVar8 = *puVar8 & 0xff000000 | uVar5;
          if (puVar10 == (uint *)(&UNK_80090994 + uVar4 * 0xa00)) {
            DAT_800a7394 = puVar10;
            return;
          }
          goto LAB_80028bc8;
        case 5:
          pbVar7 = pbVar6 + 2;
          uVar12 = ((pbVar6[1] & 3) * 2 + ((pbVar6[1] & 4) != 0) + 0x1e0) * 0x40 | 0x10;
          break;
        case 6:
          cVar3 = DAT_800b8523;
          if (pbVar6[1] != 0) {
            cVar3 = pbVar6[1] - 1;
          }
          pbVar7 = (byte *)FUN_80028840(cVar3);
          unaff_s6 = pbVar6;
          break;
        case 7:
          pbVar7 = unaff_s6 + 2;
          break;
        case 8:
          pbVar7 = pbVar6 + 1;
          uVar11 = (uint)DAT_800b8534 | ((uVar11 >> 0x10) + 0x10) * 0x10000;
        }
      }
      pbVar6 = pbVar7;
    } while (pbVar7 != DAT_800b852c);
  }
                    /* WARNING: Read-only address (ram,0x800c4416) is written */
  DAT_800a7394 = puVar10;
  return;
}
