int sub12(void) {

Work_set(3, 0);
nop();
Member_set(23, 128);
Sleep(256);
Member_copy(16, 23);
nop();
Calc(16 && 65407)
Member_set2(23, 16);
nop();
return 0;
}