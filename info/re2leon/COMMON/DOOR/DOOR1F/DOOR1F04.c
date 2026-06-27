int DOOR1F04(void) {

	Work_set(5, 1);
	nop();
	Speed_set(1, 0);
	Speed_set(7, 1024);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		nop();
		Sleep(512);
		Add_speed();
		Add_aspeed();
		Sleep(768);
	}
	for(n = 0; n < 5120; n++) {
		Add_speed();
		nop();
		Sleep(256);
		Add_speed();
		Add_aspeed();
		Sleep(512);
	}
	return 0;
}