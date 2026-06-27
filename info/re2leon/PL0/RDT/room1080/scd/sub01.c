int sub01(void) {

	if(Ck(4, 0, 0)) {
		if(Cmp(2 = 12800)) {
			Evt_exec(255, 0x1806);
		}
		switch(26) {
		case 768:
			if(Ck(5, 0, 0)) {
				Set(5, 0, 1);
				Evt_exec(255, 0x1804);
			}
		break;
		case 1536:
			if(Ck(5, 0, 0)) {
				Set(5, 0, 1);
				Evt_exec(255, 0x1804);
			}
		break;
		}
	} else {
		switch(26) {
		case 768:
			if(Ck(5, 0, 0)) {
				Set(5, 0, 1);
				Evt_exec(255, 0x1805);
			}
		break;
		case 1536:
			if(Ck(5, 0, 0)) {
				Set(5, 0, 1);
				Evt_exec(255, 0x1805);
			}
		break;
		}
		nop();
		nop();
		return 0;
}