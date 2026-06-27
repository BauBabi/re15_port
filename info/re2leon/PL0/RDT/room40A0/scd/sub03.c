int sub03(void) {

	Work_set(1, 0);
	nop();
	Pos_set(0, -24564, -1800, 12697);
	Plc_dest(0, 1290, 16660, 29859);
	while(Ck(5, 10, 0)) {
		Evt_next();
		nop();
	}
	Set(5, 10, 0);
	Plc_dest(0, 5386, 18443, -32768);
	while(Ck(5, 10, 0)) {
		Evt_next();
		nop();
	}
	Set(5, 10, 0);
	Plc_motion(0, 15, 64);
	Sleep(256);
	Plc_neck(5, 2, 0, 0, 0, 15420);
	Sleep(1536);
	Message_on(0, 1792, 0x0000);
	Xa_on(0, 8192);
	Wsleep();
	Wsleeping();
	Evt_next();
	Set(5, 11, 1);
	nop();
	Sleep(8960);
	Plc_motion(0, 15, 193);
	Sleep(5120);
	Sleep(7680);
	Plc_neck(5, 3, 0, 0, 0, 15420);
	Sleep(4096);
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	Plc_dest(0, 2058, 25615, -31073);
	while(Ck(5, 10, 0)) {
		Evt_next();
		nop();
	}
	Set(5, 10, 0);
	Plc_motion(10, 18, 64);
	Plc_neck(2, 0, 0, -28671, -14336, 15400);
	Se_on(4, 0, 1, 1, 0, 0, 0, 0);
	for(n = 0; n < 2560; n++) {
		Member_copy(16, 15);
		nop();
		Calc(16 + 16384)
		Member_set2(15, 16);
		nop();
		Member_copy(16, 11);
		nop();
		Calc(16 - 11265)
		Member_set2(11, 16);
		nop();
		Evt_next();
		nop();
	}
	for(n = 0; n < 5120; n++) {
		Plc_motion(10, 18, 64);
		Plc_cnt(8);
		Member_copy(16, 11);
		nop();
		Calc(16 - 11265)
		Member_set2(11, 16);
		nop();
		Member_copy(16, 13);
		nop();
		Calc(16 - 5120)
		Member_set2(13, 16);
		nop();
		Evt_next();
		nop();
	}
	Plc_neck(0, 0, 0, 0, 0, 15400);
	Pos_set(0, -21518, -1800, 11932);
	Member_set(15, 255);
	Member_copy(16, 0);
	nop();
	Calc(16 && 62975)
	Member_set2(0, 16);
	nop();
	Pos_set(0, -32550, -1800, 14491);
	Plc_motion(1, 6, 192);
	Evt_next();
	Plc_stop();
	while(Cmp(26 != 256)) {
		Evt_next();
		nop();
	}
	Plc_neck(4, 3, 0, 0, 0, 17990);
	Sleep(15360);
	Plc_motion(1, 6, 208);
	Sleep(12800);
	Set(5, 17, 1);
	Plc_dest(0, 2058, -12075, 14491);
	while(Ck(5, 10, 0)) {
		Evt_next();
		nop();
	}
	Set(5, 10, 0);
	Plc_dest(0, 5386, 8, -32768);
	while(Ck(5, 10, 0)) {
		Evt_next();
		nop();
	}
	Set(5, 10, 0);
	Plc_ret();
	Set(2, 7, 0);
	nop();
	return 0;
}