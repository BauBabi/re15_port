int sub10(void) {

	Work_set(3, 0);
	nop();
	Plc_neck(5, 1, 0, 0, 0, 24672);
	Sleep(2560);
	Plc_neck(0, 1, 0, 0, 0, 24672);
	Plc_dest(0, 1314, 23801, -9750);
	Sleep(2560);
	Sce_bgm_control(1, 1, 80, 65, 0);
	goto Sub18();
	Se_on(2, 14, 1, 4, 0, 0, 0, 0);
	Pos_set(0, 0, 0, 0);
	Sleep(3840);
	Se_on(2, 18, 1, 4, 0, 0, 0, 0);
	Sleep(6400);
	Se_on(2, 19, 1, 4, 0, 0, 0, 0);
	return 0;
}