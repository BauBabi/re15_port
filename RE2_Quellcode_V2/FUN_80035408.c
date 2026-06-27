/* FUN_80035408 @ 0x80035408  (Ghidra headless, raw MIPS overlay) */

void FUN_80035408(uint *param_1,int *param_2)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  SVECTOR local_58;
  VECTOR local_50;
  MATRIX MStack_40;
  
  uVar2 = param_1[0x7a];
  puVar3 = param_1 + 0x21;
  if (uVar2 != 0) {
    puVar1 = param_1 + 0x23;
    do {
      local_58.vy = *(short *)((int)param_1 + 0x76);
      local_58.vz = 0;
      local_58.vx = 0;
      RotMatrix(&local_58,&MStack_40);
      if ((*param_1 & 0x2000000) != 0) {
        MulMatrix2((MATRIX *)param_1[0x20],&MStack_40);
      }
      local_58.vx = (short)puVar1[2];
      local_58.vy = (short)puVar1[3];
      local_58.vz = *(short *)((int)puVar1 + 10);
      ApplyMatrix(&MStack_40,&local_58,&local_50);
      *(short *)(puVar1 + 5) = (short)local_50.vx;
      *(short *)((int)puVar1 + 0x16) = (short)local_50.vz;
      *puVar3 = *param_2 + local_50.vx;
      uVar2 = uVar2 - 1;
      puVar1[-1] = param_2[1] + local_50.vy;
      puVar3 = puVar3 + 8;
      *puVar1 = param_2[2] + local_50.vz;
      puVar1 = puVar1 + 8;
    } while (uVar2 != 0);
  }
  return;
}


