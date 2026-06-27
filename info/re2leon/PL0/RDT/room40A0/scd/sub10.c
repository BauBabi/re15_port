int sub10(void) {

	Work_set(4, 1);
	Set(5, 1, 1);
	nop();
	Member_set(14, 0);
	Member_set(15, 8193);
	Member_set(16, 0);
	Speed_set(1, -1793);
	Speed_set(11, -769);
	Speed_set(0, -9985);
	for(n = 0; n < 5888; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Member_set(16, 252);
	Se_on(2, 11, 1, 4, 1, 0, 0, 0);
	Speed_set(3, 16384);
	Speed_set(4, -5889);
	Speed_set(5, 0);
	Speed_set(11, 0);
	Speed_set(0, -8961);
	Speed_set(1, 0);
	for(n = 0; n < 3072; n++) {
		Add_speed();
		nop();
		Sleep(256);
	}
	Speed_set(4, 0);
	for(n = 0; n < 1024; n++) {
		Add_speed();
		nop();
		Sleep(256);
	}
	Speed_set(0, 0);
	Speed_set(4, 0);
	Speed_set(3, 16384);
	Speed_set(9, -769);
	Speed_set(2, -6401);
	Speed_set(8, 512);
	for(n = 0; n < 2048; n++) {
		Add_speed();
		Add_aspeed();
		Sleep(256);
	}
	for(n = 0; n < 2048; n++) {
		Add_speed();
		nop();
		Sleep(256);
	}
	Aot_set(3, 4, 49, 1, 129, 1271, 14591, 27655, 16390, 5, 0, 0);
	Set(5, 0, 1);
	Set(4, 155, 1);
	return 0;
}