/* CQ_add_result @ 0x8008a080  (Ghidra headless, raw MIPS overlay) */

void CQ_add_result(undefined4 param_1,undefined1 param_2,undefined4 param_3)

{
  (&DAT_800c3e84)[DAT_800c3f04 * 4] = param_1;
  (&DAT_800c3e88)[DAT_800c3f04 * 0x10] = param_2;
  rescpy(&UNK_800c3e89 + DAT_800c3f04 * 0x10,param_3);
  DAT_800c3f04 = DAT_800c3f04 + 1;
  if (7 < DAT_800c3f04) {
    DAT_800c3f04 = 0;
  }
  return;
}


