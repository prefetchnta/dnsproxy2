@echo off
set NDK_ROOT=%DEV_ROOT%\android\ndk
rem ===========================================================================
set PATH=%NDK_ROOT%\prebuilt\windows\bin;%NDK_ROOT%\toolchains\arm-linux-androideabi-4.9\prebuilt\windows\bin;%PATH%
set NDK_INC=%NDK_ROOT%\platforms\android-14\arch-arm\usr\include
set NDK_LIB=%NDK_ROOT%\platforms\android-14\arch-arm\usr\lib
set STL_LIB=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\armeabi-v7a
copy %NDK_LIB%\crtbegin_static.o
copy %NDK_LIB%\crtend_android.o
make CC="arm-linux-androideabi-gcc -march=armv7-a" SP=arm-linux-androideabi-strip
del /Q ..\bin\armeabi-v7a\*.*
move dnsproxy2 ..\bin\armeabi-v7a
del *.o /Q
rem ===========================================================================
set PATH=%NDK_ROOT%\prebuilt\windows\bin;%NDK_ROOT%\toolchains\mipsel-linux-android-4.9\prebuilt\windows\bin;%PATH%
set NDK_INC=%NDK_ROOT%\platforms\android-14\arch-mips\usr\include
set NDK_LIB=%NDK_ROOT%\platforms\android-14\arch-mips\usr\lib
set STL_LIB=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\mips
copy %NDK_LIB%\crtbegin_static.o
copy %NDK_LIB%\crtend_android.o
make CC=mipsel-linux-android-gcc SP=mipsel-linux-android-strip
del /Q ..\bin\mips\*.*
move dnsproxy2 ..\bin\mips
del *.o /Q
rem ===========================================================================
set PATH=%NDK_ROOT%\prebuilt\windows\bin;%NDK_ROOT%\toolchains\x86-4.9\prebuilt\windows\bin;%PATH%
set NDK_INC=%NDK_ROOT%\platforms\android-14\arch-x86\usr\include
set NDK_LIB=%NDK_ROOT%\platforms\android-14\arch-x86\usr\lib
set STL_LIB=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\x86
copy %NDK_LIB%\crtbegin_static.o
copy %NDK_LIB%\crtend_android.o
make CC=i686-linux-android-gcc SP=i686-linux-android-strip
del /Q ..\bin\x86\*.*
move dnsproxy2 ..\bin\x86
del *.o /Q
rem ===========================================================================
pause
