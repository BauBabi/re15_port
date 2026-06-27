int DOOR2B02(void) {

	Work_set(5, 0);
	nop();
	Speed_set(1, 0);
	Speed_set(7, -257);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		nop();
		Sleep(512);
		Add_speed();
		Add_aspeed();
		Sleep(768);
	}
	for(n = 0; n < 10240; n++) {
		Add_speed();
		nop();
		Sleep(256);
		Add_speed();
		Add_aspeed();
		Sleep(512);
	}
	Speed_set(0, 0);
	Speed_set(6, 2560);
	Speed_set(1, 0);
	Speed_set(7, 0);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	return 0;
}