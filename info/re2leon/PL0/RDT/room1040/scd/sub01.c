int sub01(void) {

	switch(26) {
	case 0:
	case 256:
		if(Ck(5, 4, 0)) {
			Set(5, 4, 1);
			Evt_exec(10, 0x1808);
		}
		if(Ck(5, 0, 0)) {
			Set(5, 0, 1);
			Set(5, 2, 0);
			goto Sub04();
		}
		if(Ck(5, 5, 0)) {
			Set(5, 5, 1);
			Evt_exec(10, 0x1809);
		}
	break;
	case 512:
		if(Ck(5, 1, 0)) {
			Set(5, 1, 1);
			Set(5, 3, 0);
			goto Sub05();
		}
	break;
	case 768:
		if(Ck(5, 2, 0)) {
			Set(5, 2, 1);
			Set(5, 0, 0);
			goto Sub06();
		}
	break;
	case 1024:
		if(Ck(5, 3, 0)) {
			Set(5, 3, 1);
			Set(5, 1, 0);
			goto Sub07();
		}
	break;
	}
	return 0;
}