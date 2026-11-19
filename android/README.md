# Android build

This is a single-module Android WebView application.

- Android Gradle Plugin: 8.7.3 (declared in `android/build.gradle`)
- Gradle: 8.9 (installed by GitHub Actions)
- JDK: 17
- Compile SDK: 35
- Application ID: `id.novia.home`

Run from this directory with Gradle 8.9 installed:

```sh
gradle --no-daemon assembleDebug
```

The debug APK is written to `app/build/outputs/apk/debug/app-debug.apk`.
