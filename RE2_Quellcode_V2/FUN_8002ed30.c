/* FUN_8002ed30 @ 0x8002ed30  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ed30(void)

{
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined1 auStack_28 [4];
  undefined1 auStack_24 [12];
  undefined1 auStack_18 [16];
  
  FUN_8002e93c(*(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20,auStack_28,DAT_800cfb7d,
               (int)DAT_800cfb7e);
  local_48 = DAT_800dcba8;
  local_44 = DAT_800dcbac;
  local_40 = DAT_800dcbb0;
  local_3c = DAT_800dcbb4;
  local_38 = DAT_800dcbb8;
  local_34 = DAT_800dcbbc;
  local_30 = DAT_800dcbc0;
  local_2c = DAT_800dcbc4;
  FUN_80076cb0(auStack_24,auStack_18);
  local_68 = DAT_8009db44;
  local_64 = DAT_8009db48;
  local_60 = DAT_8009db4c;
  local_5c = DAT_8009db50;
  local_58 = DAT_8009db54;
  local_54 = DAT_8009db58;
  local_50 = DAT_8009db5c;
  local_4c = DAT_8009db60;
  if (DAT_800cfb7d == '\x02') {
    local_60._0_2_ = (short)DAT_8009db4c;
    local_60 = CONCAT22((short)((uint)DAT_8009db4c >> 0x10),-(short)local_60);
  }
  else {
    local_68._0_2_ = (short)DAT_8009db44;
    local_68 = CONCAT22((short)((uint)DAT_8009db44 >> 0x10),-(short)local_68);
  }
  FUN_8002ce94(&local_68,&DAT_800dcba8,&DAT_800e2a90);
  DAT_800dcba8 = local_48;
  DAT_800dcbac = local_44;
  DAT_800dcbb0 = local_40;
  DAT_800dcbb4 = local_3c;
  DAT_800dcbb8 = local_38;
  DAT_800dcbbc = local_34;
  DAT_800dcbc0 = local_30;
  DAT_800dcbc4 = local_2c;
  return;
}


