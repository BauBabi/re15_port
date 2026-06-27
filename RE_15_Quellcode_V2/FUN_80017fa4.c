void FUN_80017fa4(void)

{
  undefined2 *puVar1;
  short sVar2;
  int iVar3;
  int local_18;
  int local_14;
  int local_10;
  
  puVar1 = DAT_800b52c4;
  DAT_800b52c4[2] = DAT_800b52c4[2] + 0x1e;
  local_18 = (int)(short)puVar1[0x14];
  puVar1[3] = puVar1[3] + 0x1e;
  local_14 = (int)(short)puVar1[0x15];
  local_10 = (int)(short)puVar1[0x16];
  sVar2 = FUN_8001c6e8(&local_18,0,8,0x100);
  iVar3 = FUN_80012d60(500,&local_18,0);
  if ((iVar3 != 0) || (sVar2 <= local_14)) {
    FUN_800199d4((ushort)DAT_800b52c4[0x39] + 500 | 0x30c0000,(int)(short)DAT_800b52c4[0x17],
                 &DAT_80072d4c,&local_18);
    FUN_80045024(0x3030001,&local_18);
    puVar1 = DAT_800b52c4;
    DAT_800b52c4[1] = 0;
    *puVar1 = 0;
    puVar1[10] = 0;
    puVar1[9] = 0;
    puVar1[8] = 0;
    puVar1[6] = 0;
    puVar1[5] = 0;
    puVar1[4] = 0;
    *(undefined1 *)(puVar1 + 0x37) = 0xd;
  }
  return;
}
