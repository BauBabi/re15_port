int sub16(void) {

Work_set(4, 0);
nop();
Member_set(15, 0);
Evt_next();
nop();
Member_set(15, 1280);
Evt_next();
nop();
Member_set(15, 0);
Evt_next();
nop();
return 0;
}