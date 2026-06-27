char FUN_80012d60(undefined2 param_1,undefined4 *param_2,uint param_3)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint *puVar6;
  byte bVar7;
  byte bVar8;
  char cVar9;
  int local_78 [20];
  
  bVar8 = 0;
  cVar9 = '\0';
  puVar6 = &DAT_800acc2c;
  cVar1 = DAT_800aca4e;
  bVar7 = bVar8;
  if (DAT_800aca4e != '\0') {
    do {
      bVar8 = bVar7;
      if ((*puVar6 & 1) != 0) {
        cVar1 = cVar1 + -1;
        iVar2 = FUN_8002b5d0(puVar6,param_2,param_1);
        if (iVar2 != 0) {
          bVar8 = bVar7 + 1;
          local_78[bVar7] = (int)puVar6;
        }
      }
      puVar6 = puVar6 + 0x7d;
      bVar7 = bVar8;
    } while (cVar1 != '\0');
  }
  iVar2 = FUN_8002b5d0(&DAT_800aca54,param_2,param_1);
  if (iVar2 != 0) {
    if ((DAT_800acae7 & 1) == 0) {
      DAT_800acaee = DAT_800acaee - *(short *)(&DAT_8006f418 + (param_3 & 0xff) * 2);
      if ((param_3 & 0xff) < 2) {
        FUN_800453d0(10);
        uVar3 = FUN_8001af20();
        uVar4 = FUN_8001af20();
        if ((uVar3 & 1) * (uVar4 & 1) != 0) {
          DAT_800acaec = DAT_800acaec | 2;
        }
      }
      DAT_800aca58 = 2;
      cVar1 = FUN_8001a7a8(&DAT_800aca54,*param_2,param_2[2]);
      DAT_800aca59 = cVar1 + '\x02';
      DAT_800aca5a = 0;
      DAT_800acae7 = DAT_800acae7 | 1;
      if (DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
        DAT_800aca5a = 0;
      }
    }
    cVar9 = '\x01';
  }
  if (bVar8 != 0) {
    param_3 = param_3 & 0xff;
    do {
      bVar8 = bVar8 - 1;
      iVar2 = local_78[bVar8];
      if ((*(int *)(iVar2 + 0x188) + 0x40 != *(int *)(DAT_800b52c4 + 0x74)) &&
         ((*(uint *)(iVar2 + 0x90) & 0x3000000) != 0x3000000)) {
        if (param_3 < 2) {
          FUN_800453d0(10);
        }
        *(byte *)(iVar2 + 0x93) = *(byte *)(iVar2 + 0x93) & 1;
        iVar5 = FUN_8001a7a8(iVar2,*param_2,param_2[2]);
        if (iVar5 != 0) {
          *(byte *)(iVar2 + 0x93) = *(byte *)(iVar2 + 0x93) | 0x80;
        }
        if ((*(byte *)(iVar2 + 0x93) & 1) == 0) {
          *(undefined1 *)(iVar2 + 7) = 0;
          *(undefined1 *)(iVar2 + 6) = 1;
          *(undefined *)(iVar2 + 5) = (&DAT_8006f430)[param_3];
          *(short *)(iVar2 + 0x9a) =
               *(short *)(iVar2 + 0x9a) - *(short *)(&DAT_8006f418 + param_3 * 2);
          *(byte *)(iVar2 + 0x93) = *(byte *)(iVar2 + 0x93) | 1;
          *(undefined1 *)(iVar2 + 4) = 2;
          if (*(short *)(iVar2 + 0x9a) < 0) {
            *(undefined1 *)(iVar2 + 4) = 3;
          }
        }
        else {
          *(byte *)(iVar2 + 0x93) = *(byte *)(iVar2 + 0x93) | 2;
        }
        cVar9 = cVar9 + '\x01';
      }
    } while (bVar8 != 0);
  }
  return cVar9;
}
