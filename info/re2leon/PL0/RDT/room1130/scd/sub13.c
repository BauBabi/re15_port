int sub13(void) {

	Plc_motion(1, 6, 8);
	Sleep(512);
	Work_set(3, 1);
	Plc_motion(0, 16, 0);
	nop();
	Sleep(2560);
	Set(5, 0, 1);
	Sleep(2560);
	Save(4, 53754);
	for(n = 0; n < 2560; n++) {
		Copy(16, 4);
		nop();
		Calc(16 + 7680)
		Copy(4, 16);
		nop();
		Work_copy(4, 6, 1);
		Pos_set(0, 5276, 0, 0);
		Evt_next();
		nop();
	}
	for(n = 0; n < 6912; n++) {
		Copy(16, 4);
		nop();
		Calc(16 + 7680)
		Copy(4, 16);
		nop();
		Work_copy(4, 6, 1);
		Pos_set(0, 5276, 0, 0);
		Evt_next();
		nop();
	}
	Se_on(2, 10, 0, 0, 0, 5276, 0, -11782);
	Set(5, 0, 1);
	if(Ck(1, 1, 1)) {
		Work_set(4, 4);
		nop();
		Pos_set(0, 24732, 0, 14592);
	}
	Work_set(3, 1);
	nop();
	for(n = 0; n < 4608; n++) {
		Copy(16, 4);
		nop();
		Calc(16 + 12800)
		Copy(4, 16);
		nop();
		Work_copy(4, 6, 1);
		Pos_set(0, 5276, 0, 0);
		Evt_next();
		nop();
	}
	Plc_motion(0, 16, 0);
	Plc_cnt(57);
	for(n = 0; n < 4608; n++) {
		Copy(16, 4);
		nop();
		Calc(16 + 12800)
		Copy(4, 16);
		nop();
		Work_copy(4, 6, 1);
		Pos_set(0, 5276, 0, 0);
		Evt_next();
		nop();
	}
	Plc_motion(0, 16, 0);
	Plc_cnt(57);
	for(n = 0; n < 4608; n++) {
		Copy(16, 4);
		nop();
		Calc(16 + 12800)
		Copy(4, 16);
		nop();
		Work_copy(4, 6, 1);
		Pos_set(0, 5276, 0, 0);
		Evt_next();
		nop();
	}
	Work_set(3, 1);
	nop();
	Pos_set(0, -31135, 0, 11317);
	Plc_dest(0, 1570, 29853, -9996);
	Sca_id_set(1, 0x00FC);
	return 0;
}