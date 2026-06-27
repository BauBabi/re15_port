int sub06(void) {

Member_set(2, 2048);
Member_set(3, 0);
Member_set(4, 0);
Member_set(5, 0);
Member_copy(16, 7);
nop();
Calc(16 || 64)
Member_set2(7, 16);
nop();
return 0;
}