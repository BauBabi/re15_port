int sub07(void) {

	Sca_id_set(17, 0x0000);
	Work_set(3, 0);
	nop();
	Member_set(2, 256);
	Member_set(3, 3584);
	Member_set(4, 0);
	Member_set(5, 0);
	Pos_set(0, -11541, 0, -22827);
	Dir_set(0, 0, 8, 0);
	Evt_next();
	Work_set(3, 0);
	Member_set(5, 256);
	Se_on(3, 19, 0, 3, 0, 0, 0, 0);
	Evt_exec(255, 0x1809);
	Flr_set(5, 1);
	nop();
	return 0;
}