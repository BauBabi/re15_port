int sub11(void) {

	Speed_set(2, 256);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	Sleep(7680);
	Speed_set(2, -1);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}