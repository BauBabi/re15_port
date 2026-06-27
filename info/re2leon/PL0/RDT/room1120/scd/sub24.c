int sub24(void) {

	Work_set(4, 12);
	nop();
	Member_set(12, -5910);
	Speed_set(5, 255);
	Speed_set(3, -16129);
	Speed_set(1, 25600);
	Sleep(512);
	for(n = 0; n < 1024; n++) {
		Add_speed();
		Evt_next();
	}
	Sleep(1024);
	Member_set(12, -8015);
	return 0;
}