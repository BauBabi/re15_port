/* FUN_80076cb0 @ 0x80076cb0  (Ghidra headless, raw MIPS overlay) */

void FUN_80076cb0(int *param_1,int *param_2)

{
  long lVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  MATRIX local_58;
  VECTOR local_38;
  
  DAT_800dcba8 = DAT_8009db44;
  DAT_800dcbac = DAT_8009db48;
  DAT_800dcbb0 = DAT_8009db4c;
  DAT_800dcbb4 = DAT_8009db50;
  DAT_800dcbb8 = DAT_8009db54;
  DAT_800dcbbc = DAT_8009db58;
  DAT_800dcbc0 = DAT_8009db5c;
  DAT_800dcbc4 = DAT_8009db60;
  iVar5 = *param_2 - *param_1;
  iVar3 = param_2[1] - param_1[1];
  iVar4 = param_2[2] - param_1[2];
  lVar1 = SquareRoot0(iVar5 * iVar5 + iVar3 * iVar3 + iVar4 * iVar4);
  if (lVar1 == 0) {
    trap(0x1c00);
  }
  if ((lVar1 == -1) && (iVar3 * 0x1000 == -0x80000000)) {
    trap(0x1800);
  }
  lVar2 = SquareRoot0(iVar5 * iVar5 + iVar4 * iVar4);
  if (lVar1 == 0) {
    trap(0x1c00);
  }
  if ((lVar1 == -1) && (lVar2 << 0xc == -0x80000000)) {
    trap(0x1800);
  }
  local_58.m[0][0] = (short)DAT_8009db44;
  local_58.m[0][1] = DAT_8009db44._2_2_;
  local_58.m[0][2] = (short)DAT_8009db48;
  local_58.m[1][0] = DAT_8009db48._2_2_;
  local_58.t[0] = DAT_8009db58;
  local_58.t[1] = DAT_8009db5c;
  local_58.t[2] = DAT_8009db60;
  local_58.m[2][2] = (short)((lVar2 << 0xc) / lVar1);
  local_58._18_2_ = SUB42((uint)DAT_8009db54 >> 0x10,0);
  local_58.m[2][1] = (short)((iVar3 * 0x1000) / lVar1);
  local_58.m[1][2] = -local_58.m[2][1];
  local_58.m[1][1] = local_58.m[2][2];
  local_58.m[2][0] = (short)DAT_8009db50;
  MulMatrix((MATRIX *)&DAT_800dcba8,&local_58);
  if (lVar2 != 0) {
    if (lVar2 == 0) {
      trap(0x1c00);
    }
    if ((lVar2 == -1) && (iVar4 * 0x1000 == -0x80000000)) {
      trap(0x1800);
    }
    if (lVar2 == 0) {
      trap(0x1c00);
    }
    if ((lVar2 == -1) && (iVar5 * 0x1000 == -0x80000000)) {
      trap(0x1800);
    }
    local_58.m[1][1] = (short)DAT_8009db4c;
    local_58.m[1][2] = DAT_8009db4c._2_2_;
    local_58.t[0] = DAT_8009db58;
    local_58.t[1] = DAT_8009db5c;
    local_58.t[2] = DAT_8009db60;
    local_58.m[0][0] = (short)((iVar4 * 0x1000) / lVar2);
    local_58.m[0][1] = SUB42(DAT_8009db44,2);
    local_58._18_2_ = SUB42((uint)DAT_8009db54 >> 0x10,0);
    local_58.m[2][2] = local_58.m[0][0];
    local_58.m[2][0] = (short)((iVar5 * 0x1000) / lVar2);
    local_58.m[1][0] = SUB42(DAT_8009db48,2);
    local_58.m[0][2] = -local_58.m[2][0];
    local_58.m[2][1] = (short)((uint)DAT_8009db50 >> 0x10);
    MulMatrix((MATRIX *)&DAT_800dcba8,&local_58);
  }
  local_38.vx = -*param_1;
  local_38.vy = -param_1[1];
  local_38.vz = -param_1[2];
  ApplyMatrixLV((MATRIX *)&DAT_800dcba8,&local_38,(VECTOR *)&DAT_800dcbbc);
  return;
}


