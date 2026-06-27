int sub08(void) {

	Work_set(1, 0);
	nop();
	if(Ck(1, 0, 1)) {
		Pos_set(0, 12466, 0, -340);
	} else {
		Pos_set(0, -11597, 0, 12461);
		nop();
		nop();
		Member_set(15, 8);
		Sleep(5120);
		if(Ck(1, 0, 1)) {
			Plc_motion(0, 15, 0);
		} else {
			Plc_motion(0, 15, 1);
			nop();
			nop();
			Sleep(7680);
			if(Ck(1, 0, 1)) {
				Plc_motion(0, 16, 0);
			} else {
				Plc_motion(0, 16, 1);
				nop();
				nop();
				Sleep(7168);
				Se_on(2, 5, 1, 1, 0, 28695, 14591, 0);
				Sleep(5632);
				for(n = 0; n < 512; n++) {
					if(Ck(1, 0, 1)) {
						Plc_motion(0, 17, 0);
					} else {
						Plc_motion(0, 17, 1);
						nop();
						nop();
						Sleep(4608);
						Se_on(2, 5, 1, 1, 0, 28695, 14591, 0);
						Sleep(3072);
					}
					Set(5, 2, 1);
					for(n = 0; n < 512; n++) {
						Plc_motion(0, 17, 0);
						Sleep(4608);
						Se_on(2, 5, 1, 1, 0, 28695, 14591, 0);
						Sleep(3072);
					}
					Plc_motion(0, 17, 0);
					Sleep(4608);
					Se_on(2, 5, 1, 1, 0, 28695, 14591, 0);
					Sleep(3840);
					Sleep(3072);
					Plc_cnt(50);
					Plc_motion(0, 16, 192);
					Sleep(12800);
					Plc_cnt(30);
					Plc_motion(0, 15, 192);
					Sleep(7680);
					Plc_neck(2, 0, 0, 22530, 254, 3860);
					Sleep(46080);
					Plc_neck(0, 0, 0, 0, 0, 10290);
					Sleep(15360);
					Cut_be_set(5, 1, 0);
					Plc_ret();
					nop();
					return 0;
}