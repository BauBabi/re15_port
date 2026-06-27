int sub20(void) {

Xa_on(0, 2560);
Sleep(512);
Message_on(0, 5376, 0x0000);
Work_set(3, 0);
Plc_motion(0, 15, 16);
nop();
Wsleep();
Wsleeping();
Cut_chg(0x09);
return 0;
}