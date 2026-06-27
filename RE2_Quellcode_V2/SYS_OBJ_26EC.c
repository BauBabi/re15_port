/* SYS_OBJ_26EC @ 0x800925cc  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_26EC(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int in_v0;
  int iVar1;
  undefined4 uVar2;
  int in_t0;
  int unaff_s1;
  
  while( true ) {
    if (in_v0 < 0) {
      in_v0 = in_v0 + 3;
    }
    iVar1 = param_3 * 4;
    if (in_v0 >> 2 <= param_3) break;
    uVar2 = *param_4;
    param_4 = param_4 + 1;
    param_3 = param_3 + 1;
    *(undefined4 *)(iVar1 + DAT_800b2804 * 0x60 + in_t0) = uVar2;
    in_v0 = unaff_s1;
  }
  *(undefined **)(&DAT_800e899c + DAT_800b2804 * 0x60) = &DAT_800e89a4 + DAT_800b2804 * 0x60;
  SYS_OBJ_27A8();
  return;
}


