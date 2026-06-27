package de.baubabi.types.element;

public class Rgb {

    enum transparency {
        TRANSPARENT,
        NOT_TRANSPARENT,
        SEMI_TRANSPARENT,
        NON_TRANSPARENT
    }

    int r;
    int g;
    int b;
    int stp;
    String transparent;
    String hexValueString = "";

    public Rgb(int r, int g, int b, int stp, String hexValueString) {
        this.r = r;
        this.g = g;
        this.b = b;
        this.stp = stp;
        this.hexValueString = hexValueString;
    }

    public void setTransperency() {
        if(r == 0 && g == 0 && b == 0 && stp == 0) {
            this.transparent = transparency.TRANSPARENT.name();
        }
        else if((r > 0 || g > 0 || b > 0) && stp == 0 ) {
            this.transparent = transparency.NOT_TRANSPARENT.name();
        }
        else if((r > 0 || g > 0 || b > 0) && stp == 1 ) {
            this.transparent = transparency.SEMI_TRANSPARENT.name();
        }
        else if(r == 0 && g == 0 && b == 0 && stp == 1) {
            this.transparent = transparency.NON_TRANSPARENT.name();
        }
    }
}
