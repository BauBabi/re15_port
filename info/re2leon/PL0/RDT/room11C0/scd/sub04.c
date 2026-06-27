int sub04(void) {

	Work_set(4, 0);
	nop();
	Speed_set(4, 256);
	Speed_set(10, 256);
	for(n = 0; n < 11520; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(10, -1);
	for(n = 0; n < 16640; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(10, 256);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(10, 0);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Speed_set(4, 256);
	for(n = 0; n < 20480; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Sleep(5120);
	Speed_set(4, -1);
	for(n = 0; n < 20480; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}