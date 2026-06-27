/* FUN_80018e08 @ 0x80018e08  (Ghidra headless, raw MIPS overlay) */

void FUN_80018e08(void)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  int *piVar4;
  int *piVar5;
  uint uVar6;
  int *piVar7;
  char cVar8;
  
  piVar4 = *(int **)(DAT_800ce324 + 0x60);
  if (piVar4 != (int *)0x0) {
    cVar8 = (char)piVar4[1];
    piVar7 = (int *)((int)piVar4 + *piVar4);
    piVar5 = piVar7 + 1;
    do {
      uVar6 = 0;
      puVar1 = (uint *)((int)piVar4 + *piVar7);
      uVar3 = *puVar1;
      if ((DAT_800cfb74 & 0x1000000) == 0) {
        if ((DAT_800cfbd8 & 0x2000000) == 0) {
          if ((uVar3 & 1) != 0) goto LAB_80018f18;
        }
        else if ((int)uVar3 < 0) {
LAB_80018f18:
          DAT_800cfd88 = puVar1 + 1;
          DAT_800cfd8c = (int)piVar4 + *piVar5;
        }
      }
      else {
        if (DAT_800d4828 < 3) {
          if (DAT_800d4828 < 1) {
            if (DAT_800d4828 == 0) goto LAB_80018e9c;
          }
          else if ((uVar3 & 2) != 0) {
            DAT_800cfd8c = (int)piVar4 + *piVar5;
            DAT_800cfd88 = puVar1 + 1;
          }
        }
        else if (DAT_800d4828 == 3) {
LAB_80018e9c:
          if ((uVar3 & 1) != 0) {
            DAT_800cfd8c = (int)piVar4 + *piVar5;
            DAT_800cfd88 = puVar1 + 1;
          }
        }
        uVar6 = 1;
        uVar3 = uVar3 >> 1;
      }
      uVar3 = uVar3 >> 1;
      if (uVar3 != 0) {
        do {
          if ((uVar3 & 1) != 0) {
            puVar2 = (uint *)(&DAT_800cfe18)[uVar6 & 0xff];
            if ((*puVar2 & 1) != 0) {
              puVar2[100] = (uint)(puVar1 + 1);
              *(int *)((&DAT_800cfe18)[uVar6 & 0xff] + 0x194) = (int)piVar4 + *piVar5;
            }
          }
          uVar3 = uVar3 >> 1;
          uVar6 = uVar6 + 1;
        } while (uVar3 != 0);
      }
      piVar5 = piVar5 + 2;
      piVar7 = piVar7 + 2;
      cVar8 = cVar8 + -1;
    } while (cVar8 != '\0');
  }
  return;
}


