void FUN_80026e54(int param_1,undefined4 param_2)

{
  uint uVar1;
  undefined *puVar2;
  
  if ((DAT_800b0fbe & 1) == 0) {
    puVar2 = &DAT_800107f8;
  }
  else {
    puVar2 = &DAT_800107cc;
  }
  FUN_8004ee38(param_1,puVar2,0x2a);
  *(byte *)(param_1 + 0x25) = *(char *)(param_1 + 0x25) + DAT_800b0fbd / 10;
  *(byte *)(param_1 + 0x27) = *(char *)(param_1 + 0x27) + DAT_800b0fbd % 10;
  uVar1 = FUN_80026e4c(param_2);
  FUN_8004ee38(param_1 + 0x2a,&UNK_80073628 + (uVar1 & 0xff) * 0x13,0x13);
  return;
}
