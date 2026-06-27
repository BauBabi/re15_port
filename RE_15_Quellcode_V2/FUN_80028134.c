void FUN_80028134(void)

{
  byte bVar1;
  bool bVar2;
  char cVar3;
  uint uVar4;
  byte bVar5;
  byte *pbVar6;
  byte *pbVar7;
  int iVar8;
  
  pbVar7 = DAT_800b852c;
  bVar2 = DAT_800b5456 == '\0';
  bVar1 = DAT_800b8525;
  switch(DAT_800b8521) {
  case 0:
    DAT_800b8521 = 1;
    DAT_800b8525 = (byte)(1 << bVar2);
    DAT_800b8524 = (byte)(2 << bVar2);
    DAT_800b8526 = 0;
    DAT_800b8527 = 0;
    pbVar7 = DAT_800b8528;
  case 1:
    iVar8 = 1;
    bVar5 = DAT_800b8525 - 1;
    if (DAT_800b8522 != '\0') {
      if (DAT_800b8525 == 1) goto LAB_80028250;
      if (((DAT_800ac768 & 0x4000) != 0) && (bVar5 = DAT_800b8525 - 4, DAT_800b8524 < 4)) {
        iVar8 = 2;
      }
    }
    DAT_800b8525 = bVar5;
    bVar1 = DAT_800b8525;
    bVar5 = DAT_800b8525;
    if ((char)DAT_800b8525 < '\x01') {
LAB_80028250:
      DAT_800b8525 = bVar5;
      bVar1 = *pbVar7;
      iVar8 = iVar8 + -1;
joined_r0x80028258:
      if ((bVar1 == 0) || ((byte)(*pbVar7 - 0xc) < 0xec)) goto LAB_80028434;
      bVar1 = DAT_800b8525;
      switch(*pbVar7) {
      case 1:
        pbVar7 = pbVar7 + 1;
        if (*pbVar7 == 0) {
          DAT_800b8521 = 5;
        }
        else {
          DAT_800b8521 = 6;
          bVar1 = *pbVar7 << bVar2;
        }
        goto LAB_80028748;
      case 2:
        if (pbVar7[1] == 0) {
          DAT_800b8521 = 2;
        }
        else {
          DAT_800b8521 = 3;
          DAT_800b8525 = pbVar7[1] << bVar2;
        }
        pbVar7 = pbVar7 + 2;
        bVar1 = DAT_800b8525;
        goto LAB_80028748;
      case 3:
        DAT_800b8521 = 4;
        goto LAB_80028748;
      case 4:
        pbVar6 = pbVar7 + 1;
        uVar4 = (uint)*pbVar6;
        if (uVar4 == 0) {
          pbVar7 = pbVar7 + 2;
          bVar1 = *pbVar7;
          while (bVar1 != 4) {
            bVar1 = *pbVar7;
            if ((4 < bVar1) && ((bVar1 < 7 || ((bVar1 < 0xc && (8 < bVar1)))))) {
              pbVar7 = pbVar7 + 1;
            }
            pbVar7 = pbVar7 + 1;
            bVar1 = *pbVar7;
          }
          pbVar6 = pbVar7 + 1;
          uVar4 = (uint)*pbVar6;
        }
        DAT_800b8524 = (byte)(uVar4 << bVar2);
        pbVar7 = pbVar6 + 1;
        break;
      case 5:
        DAT_800b8527 = pbVar7[1];
        pbVar7 = pbVar7 + 2;
        break;
      case 6:
        cVar3 = pbVar7[1] - 1;
        if (pbVar7[1] == 0) {
          cVar3 = DAT_800b8523;
        }
        DAT_800b8530 = pbVar7;
        pbVar7 = (byte *)FUN_80028840(cVar3);
        break;
      case 7:
        pbVar7 = DAT_800b8530 + 2;
        break;
      case 8:
        pbVar7 = pbVar7 + 1;
        break;
      case 9:
      case 10:
      case 0xb:
        goto switchD_800282b4_caseD_9;
      }
      bVar1 = *pbVar7;
      goto joined_r0x80028258;
    }
    break;
  case 2:
    if ((DAT_800ac76c & 0xc000) == 0) {
      DAT_800b8525 = DAT_800b8525 - 1;
      bVar1 = DAT_800b8525;
      if (((int)(char)DAT_800b8525 & 0x18 << bVar2) != 0) {
        FUN_800c69bc(DAT_800b8536,0x80,&DAT_80010938,0);
        FUN_800279c8();
        bVar1 = DAT_800b8525;
      }
    }
    else {
      DAT_800ac768 = 0xffff;
      DAT_800b8521 = 1;
      DAT_800b8528 = DAT_800b852c;
      DAT_800b8526 = DAT_800b8527;
      bVar1 = (byte)(1 << bVar2);
    }
    break;
  case 3:
    bVar1 = DAT_800b8525 - 1;
    if (DAT_800b8525 == 1) {
      DAT_800b8521 = 1;
      DAT_800b8528 = DAT_800b852c;
      DAT_800b8526 = DAT_800b8527;
      bVar1 = DAT_800b8524 << bVar2;
    }
    break;
  case 4:
    if ((DAT_800ac76c & 0x4000) != 0) {
      DAT_800ac768 = 0xffff;
      DAT_800b8520 = DAT_800b8520 & 0x7f;
      DAT_800aca40 = DAT_800b853c;
      FUN_8002877c();
      return;
    }
    if ((DAT_800ac76c & 0x3000) != 0) {
      DAT_800b8525 = 0;
      DAT_800b8520 = DAT_800b8520 ^ 1;
    }
    DAT_800b8525 = DAT_800b8525 - 1;
    if (((int)(char)DAT_800b8525 & 0x18 << bVar2) != 0) {
      FUN_800279c8((DAT_800b8536 + 0x10) * 0x10000 | (DAT_800b8520 & 1) * 0x46 + 0xa0,0x80,
                   &DAT_8001093c,0);
    }
    FUN_800279c8((DAT_800b8536 + 0x10) * 0x10000 | 0xae,0x80,&DAT_800c4954,0);
    bVar1 = DAT_800b8525;
    break;
  case 5:
    if ((DAT_800ac76c & 0xc000) != 0) {
      DAT_800ac768 = 0xffff;
      DAT_800aca40 = DAT_800b853c;
      DAT_800b8520 = DAT_800b8520 & 0x7f;
      return;
    }
    break;
  case 6:
    bVar1 = DAT_800b8525 - 1;
    if (DAT_800b8525 == 1) {
      DAT_800aca40 = DAT_800b853c;
      DAT_800b8520 = DAT_800b8520 & 0x7f;
      DAT_800b8525 = DAT_800b8525 - 1;
      return;
    }
  }
LAB_80028748:
  DAT_800b8525 = bVar1;
  DAT_800b852c = pbVar7;
  FUN_80028868();
  return;
switchD_800282b4_caseD_9:
  pbVar7 = pbVar7 + 1;
LAB_80028434:
  pbVar7 = pbVar7 + 1;
  bVar1 = DAT_800b8524;
  bVar5 = DAT_800b8525;
  if (iVar8 == 0) goto LAB_80028748;
  goto LAB_80028250;
}
