int sub04(void) {

	Work_set(3, 0);
	Plc_motion(1, 0, 5);
	nop();
	Se_on(3, 0, 0, 3, 0, 0, 0, 0);
	Sleep(6400);
	Plc_flg(1, 6, 0);
	Se_on(3, 0, 0, 3, 0, 0, 0, 0);
	Sleep(10240);
	Plc_flg(1, 5, 0);
	Se_on(3, 0, 0, 3, 0, 0, 0, 0);
	Sleep(6400);
	Plc_flg(1, 6, 0);
	Se_on(3, 0, 0, 3, 0, 0, 0, 0);
	Sleep(10240);
	Plc_flg(1, 5, 0);
	Se_on(3, 0, 0, 3, 0, 0, 0, 0);
	Sleep(6400);
	Plc_flg(1, 6, 0);
	Se_on(3, 0, 0, 3, 0, 0, 0, 0);
	Sleep(10240);
	Work_set(3, 0);
	Plc_motion(2, 0, 20);
	nop();
	return 0;
}