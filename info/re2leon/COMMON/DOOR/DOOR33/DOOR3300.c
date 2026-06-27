int DOOR3300(void) {

	switch(12) {
	case 0:
	case 256:
		goto Sub01();
	break;
	case 1536:
		goto Sub02();
	break;
	case 1792:
		goto Sub03();
	break;
	case 2048:
		goto Sub07();
	break;
	}
	return 0;
}