int sub16(void) {

	Cut_chg(0x01);
	Sleep(256);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 1056, 27806, -13895);
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Member_set2(0, 16);
	nop();
	Plc_neck(5, 3, 0, 0, 0, 24672);
	Sleep(256);
	Work_set(3, 0);
	nop();
	Plc_dest(0, 1057, -8547, 3251);
	Xa_on(0, 3584);
	Message_on(0, 2560, 0x0000);
	Sleep(512);
	goto Sub27();
	Work_set(1, 0);
	Plc_motion(0, 20, 0);
	nop();
	Sleep(6400);
	Wsleep();
	Wsleeping();
	return 0;
}