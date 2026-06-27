/* FUN_80035f68 @ 0x80035f68  (Ghidra headless, raw MIPS overlay) */

void FUN_80035f68(void)

{
  uint uVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  uint *puVar7;
  undefined2 *puVar8;
  int iVar9;
  uint *puVar10;
  
  if ((DAT_800cfbdc & 0x8000000) != 0) {
    return;
  }
  iVar9 = 0;
  DAT_800cfbd8 = DAT_800cfbd8 & 0xffffdfff;
  puVar10 = (uint *)0x0;
  FUN_80035994();
  FUN_80038b84(&DAT_800cfbf8);
  puVar7 = &DAT_800d0324;
  if (DAT_800d4224 != &DAT_800d0324) {
    puVar8 = &DAT_800d0430;
    do {
      if ((*puVar7 & 1) != 0) {
        if (*(char *)(puVar8 + -0x82) == '\x02') {
          FUN_80036710(puVar7);
        }
        FUN_80036e30(puVar7);
        *(char *)(puVar8 + -3) = -(char)(*(int *)(puVar8 + -0x68) / 0x708);
        if ((DAT_800cfbfc._1_1_ != '\x01') && (DAT_800cfbfc._1_1_ != '\n')) {
          *(undefined1 *)(puVar8 + 0x1c) = 0;
        }
        puVar6 = &DAT_800cfe18;
        if (DAT_800ce334 != &DAT_800cfe18) {
          do {
            if ((*(uint *)*puVar6 & 1) != 0) {
              FUN_80036ef8((uint *)*puVar6,puVar7,0);
            }
            puVar6 = puVar6 + 1;
          } while (puVar6 != DAT_800ce334);
        }
        if ((*puVar7 & 0x200) == 0) {
          if (puVar7 == DAT_800cfddc) {
            FUN_80037f60(&DAT_800cfbf8,puVar7);
          }
          else {
            iVar5 = FUN_80036ef8(&DAT_800cfbf8,puVar7,2);
            if (((iVar5 != 0) && (iVar9 = iVar9 + 1, DAT_800cfcfe == *(char *)(puVar8 + -3))) &&
               (iVar5 = FUN_80037450(&DAT_800cfbf8,puVar7), iVar5 != 0)) {
              puVar10 = puVar7;
            }
          }
        }
        else {
          FUN_80036ef8(&DAT_800cfbf8,puVar7,1);
        }
        if (((*(char *)(puVar8 + -0x82) == '\x02') && (*(char *)(puVar8 + -0x84) == '\0')) &&
           (uVar3 = FUN_8004c658(puVar7,0x4000,0x100), (uVar3 & 4) != 0)) {
          FUN_80036c5c(puVar7);
          *(undefined1 *)(puVar8 + -0x84) = 1;
        }
      }
      *puVar8 = 0;
      puVar7 = puVar7 + 0x7e;
      puVar8 = puVar8 + 0xfc;
    } while (puVar7 != DAT_800d4224);
  }
  if (((iVar9 != 1) || (puVar10 == (uint *)0x0)) ||
     (((*puVar10 & 2) != 0 || ((*puVar10 & 0x80) == 0)))) goto LAB_80036334;
  uVar3 = puVar10[0xe];
  uVar1 = puVar10[0x10];
  bVar2 = (char)puVar10[0x51] + 1;
  *(byte *)(puVar10 + 0x51) = bVar2;
  if (5 < bVar2) {
    FUN_80036ef8(&DAT_800cfbf8,puVar10,1);
    uVar4 = FUN_8004c658(puVar10,0x4000,0);
    puVar6 = &DAT_800cfe18;
    if ((uVar4 & 1) == 0) {
      iVar9 = 0;
      if (DAT_800ce334 != &DAT_800cfe18) {
        do {
          if ((*(uint *)*puVar6 & 1) != 0) {
            iVar5 = FUN_80036ef8((uint *)*puVar6,puVar10,1);
            iVar9 = iVar9 + iVar5;
          }
          puVar6 = puVar6 + 1;
        } while (puVar6 != DAT_800ce334);
      }
      puVar7 = &DAT_800d0324;
      if (DAT_800d4224 != &DAT_800d0324) {
        do {
          if ((puVar10 != puVar7) && ((*puVar7 & 1) != 0)) {
            iVar5 = FUN_80037258(puVar7,puVar10);
            iVar9 = iVar9 + iVar5;
          }
          puVar7 = puVar7 + 0x7e;
        } while (puVar7 != DAT_800d4224);
      }
      if (iVar9 == 0) {
        DAT_800cfbd8 = DAT_800cfbd8 | 0x2000;
        if ((char)puVar10[0x51] != '\x06') {
          *(undefined1 *)(puVar10 + 0x51) = 7;
          goto LAB_80036334;
        }
        goto LAB_80036308;
      }
    }
    *(undefined1 *)(puVar10 + 0x51) = 0;
  }
LAB_80036308:
  puVar10[0xe] = (int)(short)uVar3;
  puVar10[0x10] = (int)(short)uVar1;
  FUN_80036e30(puVar10);
  FUN_8004c658(puVar10,0x4000,0);
LAB_80036334:
  puVar10 = &DAT_800d0324;
  if (DAT_800d4224 != &DAT_800d0324) {
    puVar8 = &DAT_800d043e;
    do {
      if ((*puVar10 & 1) != 0) {
        FUN_80036ef8(&DAT_800cfbf8,puVar10,0);
      }
      puVar10 = puVar10 + 0x7e;
      puVar8[-1] = (short)*(undefined4 *)(puVar8 + -0x4b);
      *puVar8 = (short)*(undefined4 *)(puVar8 + -0x47);
      puVar8 = puVar8 + 0xfc;
    } while (puVar10 != DAT_800d4224);
  }
  if (DAT_800cfddc == (uint *)0x0) {
    DAT_800cfd4c = DAT_800cfd4c & 63999;
  }
  return;
}


