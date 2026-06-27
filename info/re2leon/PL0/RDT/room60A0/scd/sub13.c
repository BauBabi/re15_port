int sub13(void) {

	Light_pos_set2(2, 1, 11, 50366);
	Light_pos_set2(2, 1, 12, 3326);
	Light_pos_set2(2, 1, 13, 50366);
	Light_pos_set2(1, 1, 11, 50366);
	Light_pos_set2(1, 1, 12, 3326);
	Light_pos_set2(1, 1, 13, 50366);
	Light_color_set2(2, 1, 248, 68, 38);
	Light_color_set2(1, 1, 248, 68, 38);
	while(Ck(5, 3, 0)) {
		Light_kido_set2(0, 2, 1, 34835);
		Light_kido_set2(0, 1, 1, 34835);
		Evt_next();
		nop();
		Light_kido_set2(0, 2, 1, 47115);
		Light_kido_set2(0, 1, 1, 47115);
		Sleep(512);
		Light_kido_set2(0, 2, 1, 40975);
		Light_kido_set2(0, 1, 1, 40975);
		Evt_next();
		nop();
	}
	Light_pos_set2(1, 1, 11, 6866);
	Light_pos_set2(1, 1, 12, 24816);
	Light_pos_set2(1, 1, 13, 26311);
	Light_kido_set2(0, 1, 1, 16415);
	Light_color_set2(1, 1, 138, 138, 138);
	Light_pos_set2(2, 1, 11, 2756);
	Light_pos_set2(2, 1, 12, 24816);
	Light_pos_set2(2, 1, 13, 9921);
	Light_kido_set2(0, 2, 1, 16415);
	Light_color_set2(2, 1, 138, 138, 138);
	return 0;
}