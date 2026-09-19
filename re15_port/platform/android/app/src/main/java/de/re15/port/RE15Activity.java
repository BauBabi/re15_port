package de.re15.port;

import android.os.Bundle;
import android.view.View;

import org.libsdl.app.SDLActivity;

/**
 * RE1.5 Port - Android-Activity.
 *
 * SDLActivity (SDL2 2.28.5, aus dem per Gradle geladenen Quellbaum) uebernimmt Surface,
 * Touch, Audio und den SDL_main-Thread. Hier nur: die zu ladenden Bibliotheken (libSDL2.so,
 * libmain.so = Engine + PC-Plattformcode) und der Immersive-Vollbildmodus, damit weder
 * Status- noch Navigationsleiste Platz vom Spielbild bzw. dem Overlay-Pad nehmen.
 */
public class RE15Activity extends SDLActivity {

    @Override
    protected String[] getLibraries() {
        return new String[] { "SDL2", "main" };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        immersive();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) immersive();
    }

    private void immersive() {
        View decor = getWindow().getDecorView();
        decor.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
              | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
              | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
              | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
              | View.SYSTEM_UI_FLAG_FULLSCREEN
              | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }
}
