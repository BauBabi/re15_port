int sub08(void) {

	Cut_chg(0x0D);
	if(Ck(1, 0, 0)) {
		Ck(1, 1, 1)
		Work_set(2, 0);
		nop();
		Pos_set(0, 26316, 0, 24766);
		Dir_set(0, 0, 0, 0);
	}
	Work_set(4, 0);
	nop();
	Pos_set(0, 12405, -31741, 3276);
	goto Sub15();
	Evt_exec(255, 0x180A);
	Evt_exec(255, 0x1809);
	Speed_set(0, 27391);
	for(n = 0; n < 20480; n++) {
		Add_speed();
		Evt_next();
	}
	Se_on(2, 12, 0, 4, 0, 0, 0, 0);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	Cut_chg(0x0B);
	Evt_exec(255, 0x180A);
	Work_set(4, 0);
	nop();
	Member_copy(16, 11);
	nop();
	Calc(16 - 4135)
	Member_set2(11, 16);
	nop();
	Speed_set(0, -25345);
	for(n = 0; n < 22272; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -20225);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -15105);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -12545);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	Se_on(2, 13, 0, 4, 0, 0, 0, 0);
	Speed_set(0, -6145);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	Sleep(2560);
	Speed_set(0, 1280);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, 2560);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, 3840);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -3585);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -2305);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -1025);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, 1280);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, 2560);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -2305);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(0, -1025);
	for(n = 0; n < 1280; n++) {
		Add_speed();
		Evt_next();
	}
	Sleep(5120);
	return 0;
}