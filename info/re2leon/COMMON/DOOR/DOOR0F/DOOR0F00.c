int DOOR0F00(void) {

	switch(12) {
	case 1024:
		goto Sub01();
	break;
	case 1280:
		goto Sub02();
	break;
	default:
		goto Sub01();
	break;
	}
	return 0;
}