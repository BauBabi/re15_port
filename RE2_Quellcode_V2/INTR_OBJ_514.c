/* INTR_OBJ_514 @ 0x80086598  (Ghidra headless, raw MIPS overlay) */

void INTR_OBJ_514(void)

{
  int unaff_s1;
  int unaff_s2;
  undefined2 unaff_s3;
  
  if (unaff_s1 == 0) {
    ChangeClearPAD(unaff_s2 == 0);
    ChangeClearRCnt(3,unaff_s2 == 0);
  }
  if (unaff_s1 == 4) {
    ChangeClearRCnt(0,unaff_s2 == 0);
  }
  if (unaff_s1 == 5) {
    ChangeClearRCnt(1,unaff_s2 == 0);
  }
  if (unaff_s1 == 6) {
    ChangeClearRCnt(2,unaff_s2 == 0);
  }
  *(undefined2 *)PTR_I_MASK_800accd4 = unaff_s3;
  return;
}


