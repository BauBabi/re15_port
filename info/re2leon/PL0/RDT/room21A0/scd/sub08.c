int sub08(void) {

	Sca_id_set(20, 0x0000);
	Work_set(3, 1);
	nop();
	Member_set(2, 256);
	Member_set(3, 512);
	Member_set(4, 0);
	Member_set(5, 0);
	Member_set(12, 0);
	Work_set(4, 2);
	nop();
	Pos_set(0, -802, 0, 2787);
	Dir_set(0, 0, 255, 252);
	Se_on(3, 19, 0, 3, 0, 0, 0, 0);
	Sleep(4608);
	Se_on(2, 11, 0, 0, 0, 733, -1800, -29467);
	Flr_set(4, 1);
	nop();
	return 0;
}