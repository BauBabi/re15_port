int sub09(void) {

	Work_set(4, 0);
	nop();
	Speed_set(4, 768);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		nop();
		Sleep(512);
	}
	Speed_set(4, -513);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		nop();
		Sleep(768);
	}
	Speed_set(4, 768);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		nop();
		Sleep(1024);
	}
	Speed_set(4, 512);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		nop();
		Sleep(512);
	}
	Speed_set(4, -257);
	for(n = 0; n < 12800; n++) {
		Add_speed();
		nop();
		Sleep(512);
	}
	Speed_set(4, 512);
	for(n = 0; n < 6400; n++) {
		Add_speed();
		nop();
		Sleep(512);
	}
	Speed_set(4, 512);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		nop();
		Sleep(256);
	}
	Speed_set(4, -257);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		nop();
		Sleep(512);
	}
	Speed_set(4, 512);
	for(n = 0; n < 3840; n++) {
		Add_speed();
		nop();
		Sleep(256);
	}
	goto(0x00005DFF);
	return 0;
}