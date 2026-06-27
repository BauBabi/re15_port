/* FUN_8010fe20 @ 0x8010fe20  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fe20(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int in_v0;
  int iVar1;
  uint uVar2;
  
  DAT_800acae9 = 0;
  DAT_800acaea = 0;
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a),param_3 + 0x5a0,param_4 + -0x1408);
  func_0x80037edc(0,10);
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    DAT_800aca5a = 6;
    _DAT_800acabe = _DAT_800acabe + 0x800;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    _DAT_fffff077 = *(undefined4 *)(_DAT_800aca54 + 0x800b0000);
    uVar2 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
    func_0x8001af5c(0,0,uVar2 + 100,uVar2 + 200);
    iVar1 = *(int *)(_DAT_800ac784 + 0x188);
    *(int *)(iVar1 + 0xc84) = iVar1 + 0xec;
    *(int *)(iVar1 + 0xca8) = iVar1 + 0xac;
    *(undefined4 *)(iVar1 + 0xc44) = 0x66;
    *(undefined4 *)(iVar1 + 0xc48) = 0xfffffcd6;
    *(undefined4 *)(iVar1 + 0xc4c) = 0;
    *(undefined2 *)(iVar1 + 0xc78) = 0;
    *(undefined2 *)(iVar1 + 0xc7a) = 0;
    *(undefined2 *)(iVar1 + 0xc7c) = 0;
    func_0x80068098(iVar1 + 0xc78,iVar1 + 0xc30);
    return;
  }
  return;
}


