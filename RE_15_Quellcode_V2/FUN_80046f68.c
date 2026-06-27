int FUN_80046f68(int param_1)

{
  undefined1 *puVar1;
  byte *pbVar2;
  ushort uVar3;
  
  uVar3 = 0;
  puVar1 = (undefined1 *)(param_1 + 7);
  do {
    puVar1[-4] = 3;
    *puVar1 = 0x42;
    puVar1[-3] = 0x80;
    puVar1[-2] = 0x80;
    puVar1[-1] = 0x80;
    pbVar2 = puVar1 + 0x10;
    param_1 = param_1 + 0x20;
    *pbVar2 = 0x40;
    uVar3 = uVar3 + 1;
    puVar1[0xc] = 3;
    puVar1[0xd] = 0x80;
    puVar1[0xe] = 0x80;
    puVar1[0xf] = 0x80;
    *pbVar2 = *pbVar2 | 2;
    puVar1 = puVar1 + 0x20;
  } while (uVar3 < 0x40);
  return param_1;
}
