int sub00(void) {

	Aot_reset(0, 5, 49, 65280, 6161, 0x00);
	if(Ck(1, 1, 0)) {
		if(Ck(4, 131, 0)) {
			goto Sub20();
			Aot_reset(1, 0, 0, 0, 0, 0x00);
			goto Sub15();
			if(Ck(7, 19, 0)) {
				Evt_exec(5, 0x1806);
			}
		}
	} else {
		if(Ck(29, 13, 0)) {
			if(Ck(4, 131, 0)) {
				goto Sub20();
				Aot_reset(1, 0, 0, 0, 0, 0x00);
				goto Sub15();
				if(Ck(7, 19, 0)) {
					Evt_exec(5, 0x1806);
				}
			}
		} else {
			goto Sub16();
			nop();
			nop();
			nop();
			nop();
			goto Sub27();
			goto Sub18();
			goto Sub14();
			Evt_exec(255, 0x1802);
			Evt_exec(255, 0x181A);
			return 0;
}