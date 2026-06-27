int sub07(void) {

Set(2, 7, 1);
Work_set(1, 0);
nop();
Plc_dest(0, 1057, 1562, -19560);
do {
Evt_next();
nop();
} while(Ck(5, 33, 0));
Sleep(1024);
Plc_dest(0, 2337, 12058, -14208);
do {
Evt_next();
nop();
} while(Ck(5, 33, 0));
Member_set(15, 1796);
Set(2, 7, 0);
return 0;
}