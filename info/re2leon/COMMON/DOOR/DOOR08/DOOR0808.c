int DOOR0808(void) {

	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(6, 1280);
	for(n = 0; n < 2560; n++) {
		Add_speed();
		Add_aspeed();
		Evt_next();
		nop();
	}
	Sce_fade_set(0, 2, 7, 0, 4);
	for(n = 0; n < 7680; n++) {
		Add_speed();
		Evt_next();
	}
	return 0;
}