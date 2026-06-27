int sub19(void) {

Cut_chg(0x06);
Evt_next();
nop();
Xa_on(0, 2304);
Sleep(512);
Message_on(0, 5120, 0x0000);
Work_set(3, 0);
Plc_motion(0, 21, 0);
nop();
Plc_neck(2, 0, 0, -32513, -32768, 2056);
Sleep(7680);
Plc_neck(2, 0, 0, -32768, -32768, 2056);
Plc_motion(0, 20, 0);
Sleep(7680);
Plc_neck(2, 0, 0, -32513, -32768, 2056);
Sleep(3840);
Plc_motion(0, 16, 0);
Plc_neck(5, 1, 0, 0, 0, 8224);
Sleep(5120);
Plc_motion(0, 23, 0);
Sleep(6400);
Plc_motion(2, 9, 16);
Sleep(3840);
Se_on(2, 9, 0, 4, 0, 0, 0, 0);
Wsleep();
Wsleeping();
Sleep(12800);
return 0;
}