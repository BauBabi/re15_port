/* DSSYS_2_OBJ_C74 @ 0x8008a728  (Ghidra headless, raw MIPS overlay) */

undefined4 DSSYS_2_OBJ_C74(void)

{
  undefined4 uVar1;
  undefined4 *unaff_s1;
  undefined4 *unaff_s2;
  int unaff_s3;
  undefined1 *unaff_s4;
  int unaff_s5;
  undefined4 unaff_s6;
  undefined4 unaff_s7;
  
  while( true ) {
    uVar1 = *unaff_s2;
    unaff_s1[5] = unaff_s7;
    unaff_s1[4] = uVar1;
    unaff_s3 = unaff_s3 + 1;
    DAT_800c3e80 = DAT_800c3e80 + 1;
    if (unaff_s5 <= unaff_s3) {
      uVar1 = DSSYS_2_OBJ_D88();
      return uVar1;
    }
    unaff_s1 = (undefined4 *)CQ_last_queue();
    if (unaff_s1 == (undefined4 *)0x0) break;
    *unaff_s1 = unaff_s6;
    *(undefined1 *)(unaff_s1 + 1) = unaff_s4[0x10];
    if (unaff_s2[3] != 0) {
      parcpy((int)unaff_s1 + 5,unaff_s4 + 0x11);
      unaff_s1[3] = (int)unaff_s1 + 5;
      uVar1 = DSSYS_2_OBJ_C74();
      return uVar1;
    }
    unaff_s1[3] = 0;
    unaff_s2 = unaff_s2 + 4;
    unaff_s4 = unaff_s4 + 0x10;
  }
  return 0;
}


