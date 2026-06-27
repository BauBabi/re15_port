int sub04(void) {

Work_set(3, 1);
Set(5, 9, 1);
nop();
Member_set(23, 140);
Evt_next();
nop();
Member_copy(16, 23);
nop();
Calc(16 && 65407)
Member_set2(23, 16);
nop();
return 0;
}