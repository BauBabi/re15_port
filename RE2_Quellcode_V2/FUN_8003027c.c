/* FUN_8003027c @ 0x8003027c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800303a8) */

void FUN_8003027c(int param_1)

{
  bool bVar1;
  undefined1 uVar2;
  byte bVar3;
  char cVar4;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  int iVar8;
  
  pbVar6 = *(byte **)(param_1 + 0x5c98);
  bVar1 = DAT_800dfc1a == '\0';
  switch(*(undefined1 *)(param_1 + 0x5c8d)) {
  case 0:
    *(undefined1 *)(param_1 + 0x5c8d) = 1;
    uVar2 = (undefined1)(1 << bVar1);
    *(undefined1 *)(param_1 + 0x5c91) = uVar2;
    *(undefined1 *)(param_1 + 0x5c90) = uVar2;
    *(undefined1 *)(param_1 + 0x5c92) = 0;
    *(undefined1 *)(param_1 + 0x5c93) = 0;
    pbVar6 = *(byte **)(param_1 + 0x5c94);
    DAT_800cfbd8 = DAT_800cfbd8 & 0xffffffbf;
    DAT_800cfbdc = DAT_800cfbdc | *(uint *)(param_1 + 0x5cb0);
  case 1:
    iVar8 = 1;
    cVar4 = *(char *)(param_1 + 0x5c91);
    *(char *)(param_1 + 0x5c91) = cVar4 + -1;
    if ((*(char *)(param_1 + 0x5c8e) != '\0') && (bVar3 = cVar4 - 4, (DAT_800ce30c & 0x3000) != 0))
    {
      *(byte *)(param_1 + 0x5c91) = bVar3;
      if (0 < (int)((uint)bVar3 << 0x18)) break;
      bVar3 = *(byte *)(param_1 + 0x5c90);
      if (bVar3 == 2) {
        iVar8 = 2;
      }
      else if (bVar3 < 3) {
        iVar8 = 4;
      }
      else {
        iVar8 = 2;
        if (bVar3 != 3) {
          iVar8 = 1;
        }
      }
    }
    if ('\0' < *(char *)(param_1 + 0x5c91)) {
      *(byte **)(param_1 + 0x5c98) = pbVar6;
      goto LAB_8003081c;
    }
LAB_800305b4:
    while ((*pbVar6 != 0 && (*pbVar6 < 0xee))) {
LAB_800305cc:
      if (*pbVar6 != 0) {
        *(byte *)(param_1 + 0x5c8c) = *(byte *)(param_1 + 0x5c8c) | 0x40;
      }
      pbVar6 = pbVar6 + 1;
      if ((*(byte *)(param_1 + 0x5c8c) & 0x40) != 0) {
        iVar8 = iVar8 + -1;
      }
      if (iVar8 == 0) {
        *(undefined1 *)(param_1 + 0x5c91) = *(undefined1 *)(param_1 + 0x5c90);
        goto switchD_800302c8_default;
      }
    }
    bVar3 = *pbVar6;
    if (bVar3 == 0xf8) {
      *(byte **)(param_1 + 0x5c9c) = pbVar6;
      bVar3 = pbVar6[1];
      if (bVar3 == 0) {
        bVar3 = *(byte *)(param_1 + 0x5c8f);
      }
      pbVar6 = (byte *)FUN_80030b9c(bVar3);
      goto LAB_800305b4;
    }
    if (bVar3 < 0xf9) {
      if (bVar3 < 0xf1) {
        if (0xed < bVar3) {
          pbVar6 = pbVar6 + 1;
          goto LAB_800305cc;
        }
        if (bVar3 != 0) goto LAB_800305b4;
      }
      else if (bVar3 != 0xf5) {
        if (bVar3 == 0xf7) {
          pbVar6 = (byte *)(*(int *)(param_1 + 0x5c9c) + 2);
        }
        goto LAB_800305b4;
      }
LAB_8003055c:
      pbVar6 = pbVar6 + 1;
      goto LAB_800305b4;
    }
    if (bVar3 == 0xfb) {
      *(undefined1 *)(param_1 + 0x5c8d) = 4;
      *(char *)(param_1 + 0x5caa) = (char)(8 << bVar1);
      break;
    }
    if (bVar3 < 0xfc) {
      if (bVar3 == 0xf9) {
        pbVar7 = pbVar6 + 1;
        pbVar6 = pbVar6 + 2;
        *(byte *)(param_1 + 0x5c93) = *pbVar7;
      }
      else if (bVar3 == 0xfa) {
        pbVar7 = pbVar6 + 1;
        if (*pbVar7 == 0) {
          pbVar6 = pbVar6 + 2;
          bVar3 = *pbVar6;
          while (bVar3 != 0xfa) {
            if ((0xed < bVar3) && ((bVar3 < 0xf1 || ((bVar3 < 0xfa && (0xf7 < bVar3)))))) {
              pbVar6 = pbVar6 + 1;
            }
            pbVar6 = pbVar6 + 1;
            bVar3 = *pbVar6;
          }
          pbVar7 = pbVar6 + 1;
        }
        uVar5 = 1;
        if (1 < *pbVar7) {
          uVar5 = (uint)(*pbVar7 >> 1);
        }
        *(char *)(param_1 + 0x5c90) = (char)(uVar5 << bVar1);
        pbVar6 = pbVar7;
        goto LAB_8003055c;
      }
      goto LAB_800305b4;
    }
    if (bVar3 == 0xfd) {
      if (pbVar6[1] == 0) {
        *(undefined1 *)(param_1 + 0x5c8d) = 2;
      }
      else {
        *(undefined1 *)(param_1 + 0x5c8d) = 3;
        *(byte *)(param_1 + 0x5c91) = pbVar6[1] << bVar1;
      }
      pbVar6 = pbVar6 + 2;
      break;
    }
    if (bVar3 < 0xfd) goto LAB_8003055c;
    if (bVar3 != 0xfe) goto LAB_800305b4;
    pbVar6 = pbVar6 + 1;
    if (*pbVar6 == 0) {
      *(undefined1 *)(param_1 + 0x5c8d) = 5;
    }
    else {
      *(undefined1 *)(param_1 + 0x5c8d) = 6;
      *(byte *)(param_1 + 0x5c91) = *pbVar6 << bVar1;
    }
    break;
  case 2:
    if ((DAT_800ce310 & 0x1000) == 0) {
      cVar4 = *(char *)(param_1 + 0x5c91) + -1;
      *(char *)(param_1 + 0x5c91) = cVar4;
      if (((int)cVar4 & 0x18 << bVar1) != 0) {
        if ((*(ushort *)(param_1 + 0x5ca8) & 0x400) == 0) {
          FUN_8002fa28(0x99,*(ushort *)(param_1 + 0x5ca6) + 0x1c,0,3,&DAT_80010a1c,0);
          *(byte **)(param_1 + 0x5c98) = pbVar6;
        }
        else {
          FUN_8002fa28(0x6b,*(ushort *)(param_1 + 0x5ca6) + 0x2a,0,3,&DAT_80010a1c,0);
          *(byte **)(param_1 + 0x5c98) = pbVar6;
        }
        goto LAB_8003081c;
      }
    }
    else {
      DAT_800ce30c = 0xffff;
      *(undefined1 *)(param_1 + 0x5c8d) = 1;
      uVar2 = *(undefined1 *)(param_1 + 0x5c93);
      cVar4 = (char)(1 << bVar1);
      *(byte **)(param_1 + 0x5c94) = pbVar6;
LAB_8003071c:
      *(char *)(param_1 + 0x5c91) = cVar4;
      *(undefined1 *)(param_1 + 0x5c92) = uVar2;
    }
    break;
  case 3:
    cVar4 = *(char *)(param_1 + 0x5c91);
    *(char *)(param_1 + 0x5c91) = cVar4 + -1;
    if (cVar4 == '\x01') {
      *(undefined1 *)(param_1 + 0x5c8d) = 1;
      uVar2 = *(undefined1 *)(param_1 + 0x5c93);
      *(byte **)(param_1 + 0x5c94) = pbVar6;
      cVar4 = *(char *)(param_1 + 0x5c90) << bVar1;
      goto LAB_8003071c;
    }
    break;
  case 4:
    uVar5 = (uint)pbVar6[1];
    if ((DAT_800cfb74 & 0x840) == 0x40) {
      FUN_80030844(param_1,&UNK_8009df10 + (uVar5 & 0xf) * 6,uVar5,1);
      *(byte **)(param_1 + 0x5c98) = pbVar6;
    }
    else {
      FUN_80030844(param_1,&UNK_8009dee4 + (uVar5 & 0xf) * 6,uVar5,0);
      *(byte **)(param_1 + 0x5c98) = pbVar6;
    }
    goto LAB_8003081c;
  case 5:
    if ((DAT_800ce310 & 0x3000) != 0) {
      DAT_800ce30c = 0xffff;
LAB_800307e0:
      *(byte *)(param_1 + 0x5c8c) = *(byte *)(param_1 + 0x5c8c) & 0x7f;
      DAT_800cfbdc = *(undefined4 *)(param_1 + 0x5cac);
      return;
    }
    break;
  case 6:
    cVar4 = *(char *)(param_1 + 0x5c91);
    *(char *)(param_1 + 0x5c91) = cVar4 + -1;
    if (cVar4 == '\x01') goto LAB_800307e0;
  }
switchD_800302c8_default:
  *(byte **)(param_1 + 0x5c98) = pbVar6;
LAB_8003081c:
  FUN_80030c9c(param_1);
  return;
}


