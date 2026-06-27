int sub25(void) {

	Light_color_set2(5, 0, 78, 168, 58);
	while(Ck(5, 19, 0)) {
		Light_kido_set2(0, 5, 0, 53255);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 47115);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 40975);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 47115);
		Evt_next();
		nop();
	}
	Light_color_set2(5, 0, 168, 48, 28);
	while(Ck(5, 19, 1)) {
		Light_kido_set2(0, 5, 0, 53255);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 47115);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 40975);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 34835);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 28695);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 34835);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 40975);
		Evt_next();
		nop();
		Light_kido_set2(0, 5, 0, 47115);
		Evt_next();
		nop();
	}
	goto(0x000080FF);
	return 0;
}