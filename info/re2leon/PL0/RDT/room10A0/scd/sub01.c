int sub01(void) {

	if(Ck(1, 6, 0)) {
		if(Ck(4, 0, 0)) {
			if(Cmp(26 = 512)) {
				if(Ck(5, 0, 0)) {
					Set(5, 0, 1);
					Evt_exec(255, 0x1803);
				}
			}
			if(Cmp(26 >= 768)) {
				if(Ck(5, 0, 0)) {
					Set(5, 0, 1);
					Evt_exec(255, 0x1804);
				}
			}
		} else {
			nop();
			nop();
		}
		return 0;
}