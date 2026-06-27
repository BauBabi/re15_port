int sub08(void) {

Sca_id_set(0, 0x0000);
Work_set(3, 2);
nop();
Pos_set(0, 6602, 0, -3270);
Member_copy(16, 7);
nop();
Calc(16 - 128)
Member_set2(7, 16);
nop();
Work_set(3, 3);
nop();
Pos_set(0, -20571, 0, 14906);
Member_copy(16, 7);
nop();
Calc(16 - 128)
Member_set2(7, 16);
nop();
return 0;
}