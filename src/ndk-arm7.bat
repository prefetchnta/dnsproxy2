@echo off
set NDK_ROOT=%DEV_ROOT%\android\ndk
set PATH=%NDK_ROOT%\prebuilt\windows\bin;%NDK_ROOT%\toolchains\arm-linux-androideabi-4.9\prebuilt\windows\bin;%PATH%
set NDK_INC=%NDK_ROOT%\platforms\android-14\arch-arm\usr\include
set NDK_LIB=%NDK_ROOT%\platforms\android-14\arch-arm\usr\lib
set STL_LIB=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\armeabi-v7a
copy %NDK_LIB%\crtbegin_static.o
copy %NDK_LIB%\crtend_android.o
make -fAndroid.mk
del *.o /Q
pause
