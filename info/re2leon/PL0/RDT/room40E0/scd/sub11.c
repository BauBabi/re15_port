int sub11(void) {

	Evt_next();
	Work_set(4, 0);
	Pos_set(0, -30701, 0, 3276);
	Cut_chg(0x0B);
	Evt_exec(255, 0x180A);
	goto Sub16();
	Se_on(2, 12, 0, 4, 0, 0, 0, 0);
	Work_set(4, 0);
	nop();
	Speed_set(0, -25345);
	for(n = 0; n < 22272; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -20225);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -20225);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	Sce_fade_set(1, 2, 7, 255, 2);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Se_on(2, 16, 0, 4, 0, 0, 0, 0);
	Sleep(5120);
	return 0;
}