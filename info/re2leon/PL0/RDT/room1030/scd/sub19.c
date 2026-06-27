int sub19(void) {

	switch(26) {
	case 512:
	case 768:
	case 1536:
	case 1024:
	case 1280:
		if(Ck(5, 1, 0)) {
			Set(5, 1, 1);
			Set(5, 2, 0);
			Work_set(3, 0);
			nop();
			Member_set(12, 0);
			Member_copy(16, 0);
			nop();
			Calc(16 && 65531)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 && 63487)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 && 65343)
			Member_set2(7, 16);
			nop();
		}
	break;
	case 0:
		if(Ck(5, 17, 1)) {
			Set(5, 17, 0);
			goto Sub11();
		}
		if(Ck(5, 2, 0)) {
			Set(5, 2, 1);
			Set(5, 1, 0);
			Work_set(3, 0);
			nop();
			Member_set(12, 131);
			Member_copy(16, 0);
			nop();
			Calc(16 || 4)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 || 2048)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 || 128)
			Member_set2(7, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 || 64)
			Member_set2(7, 16);
			nop();
		}
	break;
	case 256:
	case 2560:
	case 2816:
		if(Ck(5, 17, 0)) {
			Set(5, 17, 1);
			goto Sub12();
		}
	case 1792:
	case 2048:
	case 2304:
	case 3072:
	case 3328:
	case 3584:
	case 3840:
		if(Ck(5, 2, 0)) {
			Set(5, 2, 1);
			Set(5, 1, 0);
			Work_set(3, 0);
			nop();
			Member_set(12, 131);
			Member_copy(16, 0);
			nop();
			Calc(16 || 4)
			Member_set2(0, 16);
			nop();
			Member_copy(16, 1);
			nop();
			Calc(16 || 2048)
			Member_set2(1, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 || 128)
			Member_set2(7, 16);
			nop();
			Member_copy(16, 7);
			nop();
			Calc(16 || 64)
			Member_set2(7, 16);
			nop();
		}
	break;
	}
	Evt_next();
	nop();
	goto(0x00008AFE);
	return 0;
}