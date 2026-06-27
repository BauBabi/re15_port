void FUN_8001b3f8(void)

{
  uint uVar1;
  uint *puVar2;
  uint uVar3;
  int *piVar4;
  int *piVar5;
  uint uVar6;
  int *piVar7;
  char cVar8;
  
  piVar5 = *(int **)(DAT_800ac778 + 0x5c);
  uVar6 = 0;
  if (piVar5 != (int *)0x0) {
    cVar8 = (char)piVar5[1];
    piVar7 = (int *)((int)piVar5 + *piVar5);
    piVar4 = piVar7 + 1;
    do {
      puVar2 = (uint *)((int)piVar5 + *piVar7);
      uVar3 = *puVar2 >> 1;
      if ((*puVar2 & 1) != 0) {
        DAT_800acbd4 = puVar2 + 1;
        DAT_800acbd8 = (int)piVar5 + *piVar4;
      }
      if (uVar3 != 0) {
        do {
          uVar1 = uVar3 & 1;
          uVar3 = uVar3 >> 1;
          if (uVar1 != 0) {
            (&DAT_800acdac)[(uVar6 & 0xff) * 0x7d] = puVar2 + 1;
            (&DAT_800acdb0)[(uVar6 & 0xff) * 0x7d] = (int)piVar5 + *piVar4;
          }
          uVar6 = uVar6 + 1;
        } while (uVar3 != 0);
      }
      piVar4 = piVar4 + 2;
      cVar8 = cVar8 + -1;
      piVar7 = piVar7 + 2;
    } while (cVar8 != '\0');
  }
  return;
}
