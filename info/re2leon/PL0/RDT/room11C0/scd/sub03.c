int sub03(void) {

	Evt_exec(255, 0x180B);
	Evt_exec(255, 0x180C);
	Evt_exec(255, 0x1809);
	Evt_exec(255, 0x180A);
	Cut_chg(0x09);
	Work_set(4, 0);
	nop();
	Speed_set(0, -3071);
	Speed_set(2, -3071);
	for(n = 0; n < 16384; n++) {
		Add_speed();
		Evt_next();
	}
	Speed_set(8, -2305);
	Speed_set(6, -2305);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Cut_chg(0x07);
	Member_set(12, -18240);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Evt_exec(255, 0x1804);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(8, -1);
	Speed_set(6, -1);
	for(n = 0; n < 25600; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Work_set(4, 2);
	nop();
	Member_copy(16, 1);
	nop();
	Calc(16 && 63487)
	Member_set2(1, 16);
	nop();
	Work_set(4, 0);
	Cut_chg(0x08);
	nop();
	Member_set(12, -14130);
	Speed_set(8, -257);
	Speed_set(6, -257);
	for(n = 0; n < 25600; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(8, 512);
	Speed_set(6, 512);
	for(n = 0; n < 24576; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(5, -257);
	for(n = 0; n < 24576; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(5, 0);
	Speed_set(8, -769);
	Speed_set(6, -769);
	for(n = 0; n < 12288; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Evt_exec(255, 0x1805);
	Speed_set(8, -257);
	Speed_set(6, -257);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(2, -4865);
	Speed_set(0, -4865);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Sleep(2560);
	Se_on(2, 12, 1, 4, 0, 0, 0, 0);
	Cut_chg(0x0A);
	Sleep(15360);
	return 0;
}