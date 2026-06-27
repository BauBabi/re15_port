int sub07(void) {

	Work_set(1, 0);
	nop();
	Pos_set(0, -23867, 0, -18236);
	Member_set(15, 28677);
	Plc_motion(1, 6, 64);
	Plc_cnt(24);
	Sleep(256);
	Se_on(2, 15, 1, 1, 0, 0, 0, 0);
	Sleep(15360);
	Se_on(2, 11, 1, 1, 0, 0, 0, 0);
	Evt_exec(255, 0x180D);
	Evt_exec(255, 0x1809);
	Plc_motion(1, 6, 192);
	Sleep(6400);
	Plc_dest(0, 1794, -13369, 10696);
	while(Ck(5, 2, 0)) {
		Evt_next();
		nop();
	}
	Plc_ret();
	nop();
	return 0;
}