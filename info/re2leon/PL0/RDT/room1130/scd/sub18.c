int sub18(void) {

	Cut_chg(0x01);
	Work_set(1, 0);
	nop();
	Plc_neck(1, 114, 153, -1800, -1846, 24672);
	Plc_dest(0, 2336, 23197, -1846);
	Sleep(1792);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 1568, 23197, -1846);
	Evt_next();
	nop();
	Xa_on(0, 4096);
	Message_on(0, 3072, 0x0000);
	Sleep(512);
	Work_set(1, 0);
	Plc_motion(0, 19, 0);
	nop();
	Sleep(9984);
	Evt_next();
	Plc_motion(0, 19, 144);
	nop();
	Sleep(2304);
	Evt_next();
	Plc_motion(0, 19, 16);
	Plc_cnt(25);
	nop();
	Wsleep();
	Wsleeping();
	return 0;
}