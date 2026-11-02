package org.hurrican.game;

import android.os.Bundle;
import org.libsdl.app.SDLActivity;

public class HurricanActivity extends SDLActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "hurrican",
            "SDL2",
            "SDL2_image",
            "SDL2_mixer"
        };
    }
}
