void FUN_80013278(int param_1,uint param_2)

{
  int iVar1;
  ushort *puVar2;
  int iVar3;
  uint local_18 [2];
  
  iVar3 = 0;
  if (param_1 == 0) {
    puVar2 = (ushort *)0x0;
  }
  else {
    puVar2 = &DAT_8006feac;
  }
  local_18[0] = 0;
  if (param_2 != 0) {
    do {
      if ((*puVar2 & 0x8000) != 0) {
        iVar3 = iVar3 + 1;
        puVar2 = puVar2 + 1;
      }
      local_18[0] = local_18[0] + 1;
      puVar2 = puVar2 + 1;
    } while (local_18[0] < param_2);
  }
  DAT_800bed8c = 1;
  DAT_800bed8d = (undefined1)iVar3;
  iVar1 = FUN_800130c4(0,local_18);
  DAT_800b52d0 = (*puVar2 & 0x7fff) * 0x10 + iVar3 + iVar1;
  DAT_800b5270 = (puVar2[1] & 0x7fff) * 0x10 + iVar3 + iVar1;
  CdIntToPos(DAT_800b52d0,(CdlLOC *)&DAT_800bee6c);
  DAT_800bed88 = 0;
  FUN_80013404();
  return;
}
