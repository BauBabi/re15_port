void FUN_80026f90(void)

{
  FUN_8006e170();
  DAT_8008f65c = OpenEvent(0xf4000001,4,0x2000,0);
  DAT_8008f660 = OpenEvent(0xf4000001,0x100,0x2000,0);
  DAT_8008f664 = OpenEvent(0xf4000001,0x2000,0x2000,0);
  DAT_8008f668 = OpenEvent(0xf4000001,0x8000,0x2000,0);
  DAT_8008f66c = OpenEvent(0xf0000011,4,0x2000,0);
  DAT_8008f670 = OpenEvent(0xf0000011,0x100,0x2000,0);
  DAT_8008f674 = OpenEvent(0xf0000011,0x8000,0x2000,0);
  FUN_8006e468();
  EnableEvent(DAT_8008f65c);
  EnableEvent(DAT_8008f660);
  EnableEvent(DAT_8008f664);
  EnableEvent(DAT_8008f668);
  EnableEvent(DAT_8008f66c);
  EnableEvent(DAT_8008f670);
  EnableEvent(DAT_8008f674);
  return;
}
