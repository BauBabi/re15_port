int sub15(void) {

	Evt_next();
	nop();
	switch(26) {
	case 0:
	case 256:
	case 512:
		goto Sub16();
	break;
	case 768:
	case 1024:
	case 1280:
	case 1536:
		goto Sub18();
	break;
	case 1792:
	case 2048:
	case 2304:
		goto Sub20();
	break;
	}
	Set(5, 16, 0);
	return 0;
}