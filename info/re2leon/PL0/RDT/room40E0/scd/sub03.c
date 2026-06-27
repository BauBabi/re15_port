int sub03(void) {

	Work_set(1, 0);
	nop();
	if(Member_cmp(13 > -32575)) {
		Save(4, 64000);
	} else {
		if(Member_cmp(13 < -26435)) {
			Save(4, 38415);
		}
		Plc_dest(0, 5408, 1280, 25600);
		nop();
		nop();
		do {
			Evt_next();
			nop();
		} while(Ck(5, 32, 0));
		Plc_neck(1, 52, 214, 31996, -16705, 8224);
		Sleep(5120);
		Plc_neck(4, 2, 0, 0, 0, 8192);
		Sleep(7680);
		Plc_dest(0, 2336, 17876, 17856);
		do {
			Evt_next();
			nop();
		} while(Ck(5, 32, 0));
		Plc_dest(0, 1056, 17876, 17856);
		if(Member_cmp(13 > -22080)) {
			do {
				Evt_next();
				nop();
			} while(Member_cmp(13 > -22080));
		} else {
			do {
				Evt_next();
				nop();
			} while(Member_cmp(13 < -7745));
			nop();
			nop();
			for(n = 0; n < 2560; n++) {
				if(Member_cmp(11 > -7725)) {
				break;
			}
			Evt_next();
			nop();
		}
		Plc_dest(0, 5408, 0, 25600);
		do {
			Evt_next();
			nop();
		} while(Ck(5, 32, 0));
		Evt_exec(255, 0x180D);
		Plc_motion(0, 15, 8);
		Sleep(11520);
		Plc_motion(0, 15, 136);
		Sleep(11520);
		return 0;
}