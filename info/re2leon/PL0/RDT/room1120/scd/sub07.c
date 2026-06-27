int sub07(void) {

	Message_on(0, 256, 0xFFFF);
	Evt_next();
	nop();
	if(Ck(11, 30, 0)) {
		Work_copy(8, 5, 0);
		Se_on(2, 11, 1, 4, 0, 0, 0, 0);
		if(Ck(4, 23, 1)) {
			if(Ck(11, 31, 0)) {
				if(Cmp(7 = 0)) {
					Save(11, 512);
					return Sub00();
				}
				Copy(16, 7);
				nop();
				Calc(16 - 256)
				Copy(7, 16);
				nop();
				if(Work_copy(7, 2, 0);) {
					Ck(5, 0, 0)
					Save(11, 0);
					return Sub00();
				} else {
					Save(11, 512);
					return Sub00();
					nop();
					nop();
				} else {
					if(Cmp(7 = 2048)) {
						Save(11, 768);
						return Sub00();
					}
					Copy(16, 7);
					nop();
					Calc(16 + 256)
					Copy(7, 16);
					nop();
					if(Work_copy(7, 2, 0);) {
						Ck(5, 0, 0)
						Save(11, 256);
						return Sub00();
					} else {
						Save(11, 768);
						return Sub00();
						nop();
						nop();
						nop();
						nop();
					}
				} else {
					Se_on(4, 5, 1, 1, 0, 0, 0, 0);
					nop();
					nop();
					Save(11, 1024);
					return 0;
}