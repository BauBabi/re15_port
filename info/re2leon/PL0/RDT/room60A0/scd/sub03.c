int sub03(void) {

	Work_set(1, 0);
	nop();
	Member_copy(16, 15);
	nop();
	Calc(16 && 65295)
	Copy(7, 16);
	nop();
	if(Member_cmp(9 = 1280)) {
		if(Cmp(7 <= -29686)) {
			if(Cmp(7 >= 13320)) {
				Evt_exec(255, 0x1805);
			}
		}
	}
	if(Member_cmp(9 = 1536)) {
		if(Cmp(7 <= 13320)) {
			if(Cmp(7 >= 16390)) {
				Evt_exec(255, 0x1805);
			}
		}
	}
	if(Member_cmp(9 = 1792)) {
		if(Cmp(7 <= 30725)) {
			if(Cmp(7 >= -6141)) {
				Evt_exec(255, 0x1805);
			}
		}
	}
	return 0;
}