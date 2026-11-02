# Hurrican Android SDL port

This repo already contains the required SDL2 and GLES path for Android; the app layer is the remaining missing piece.

To build:

1. Install Android Studio / Android SDK / NDK.
2. Open the `android/` folder in Android Studio.
3. Ensure the SDL Android package or SDL submodule is available for the project.
4. Build the app target.

The native game project itself is in `Hurrican/` and uses SDL2 with `RENDERER=GLES2` on Android.

Important notes:
- The game expects its asset folders (`data/` and `lang/`) to be copied into the APK or loaded from external storage.
- Touch controls should be layered on top of the existing SDL input path as a follow-up task.
