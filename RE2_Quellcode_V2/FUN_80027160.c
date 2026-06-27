/* FUN_80027160 @ 0x80027160  (Ghidra headless, raw MIPS overlay) */

void FUN_80027160(int param_1,uint *param_2,uint param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  MATRIX *m;
  uint *puVar3;
  char cVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  undefined1 auStack_58 [32];
  MATRIX MStack_38;
  
  puVar5 = auStack_58;
  puVar6 = auStack_58;
  if ((param_3 & 0x2000000) == 0) {
    RotMatrix((SVECTOR *)(param_1 + 0x74),(MATRIX *)(param_1 + 0x24));
    uVar1 = param_3 & 0x800;
  }
  else {
    m = &MStack_38;
    RotMatrix((SVECTOR *)(param_1 + 0x74),m);
    if ((param_3 & 0x1000000) == 0) {
      gte_SetRotMatrix(*(MATRIX **)(param_1 + 0x80));
      gte_ldclmv(m);
      gte_rtir();
      gte_stclmv((MATRIX *)(param_1 + 0x24));
      gte_ldclmv((MATRIX *)(puVar5 + 0x22));
      gte_rtir();
      gte_stclmv((MATRIX *)(param_1 + 0x26));
      gte_ldclmv((MATRIX *)(puVar5 + 0x24));
      gte_rtir();
      gte_stclmv((MATRIX *)(param_1 + 0x28));
      uVar1 = param_3 & 0x800;
      puVar6 = puVar5;
    }
    else {
      MStack_38.t[0] = *(long *)(param_1 + 0x38);
      MStack_38.t[1] = *(long *)(param_1 + 0x3c);
      MStack_38.t[2] = *(long *)(param_1 + 0x40);
      FUN_8002ce94(*(undefined4 *)(param_1 + 0x80),m,param_1 + 0x24);
      uVar1 = param_3 & 0x800;
      puVar6 = auStack_58;
    }
  }
  if (uVar1 != 0) {
    *(int *)(puVar6 + 0x10) = (int)*(short *)(param_1 + 0x176);
    *(int *)(puVar6 + 0x14) = (int)*(short *)(param_1 + 0x176);
    *(int *)(puVar6 + 0x18) = (int)*(short *)(param_1 + 0x176);
    ScaleMatrix((MATRIX *)(param_1 + 0x24),(VECTOR *)(puVar6 + 0x10));
  }
  FUN_80076f88(param_2 + 0x17);
  SetColorMatrix((MATRIX *)&DAT_8009db84);
  cVar4 = *(char *)(param_1 + 0x107);
  iVar2 = FUN_8002c820(param_4,DAT_800ce338);
  if (iVar2 == 0) {
    puVar3 = param_2 + 0x12;
  }
  else {
    puVar3 = param_2 + 0x12;
    if ((param_3 & 0x80000) == 0) {
      do {
        if ((*param_2 & 1) != 0) {
          FUN_80027434(param_1,param_2,*param_2,param_2 + 0x12);
        }
        cVar4 = cVar4 + -1;
        param_2 = param_2 + 0x2b;
      } while (cVar4 != '\0');
      return;
    }
  }
  do {
    uVar1 = *param_2;
    if ((uVar1 & 0x9000) == 0) {
      if ((uVar1 & 1) != 0) {
        FUN_80027ff0(param_1,param_2,uVar1,puVar3);
      }
    }
    else if ((uVar1 & 1) != 0) {
      FUN_80027434(param_1,param_2,uVar1,puVar3);
    }
    puVar3 = puVar3 + 0x2b;
    cVar4 = cVar4 + -1;
    param_2 = param_2 + 0x2b;
  } while (cVar4 != '\0');
  return;
}


