Versions for old Linux devices (ARMv7-A (Cortex-A8); glibc 2.5 (2006)):
  sunvox_armel
  sunvox_*_armel.deb (MAEMO)

Versions for modern Linux devices (ARMHF; ARMv6; glibc 2.28 (2018)):
  sunvox        - 32-bit (floating point) audio engine
  sunvox_lofi   - 16-bit (fixed point 4.12) audio engine for slow devices
  sunvox_opengl - 32-bit audio engine + OpenGL graphics; not always faster :(

For the 64-bit version, see the linux_arm64 folder.


Low FPS on Raspberry Pi? Please follow the instructions below.

sunvox_opengl (with OpenGL):
  * in the raspi-config -> Advanced Options:
    * set GL driver to "full KMS";
    * enable Glamor;

sunvox (without OpenGL):
  * add the "softrender" option to the SunVox config (~/.config/SunVox/sunvox_config.ini);
  * in the raspi-config -> Advanced Options:
    * disable Glamor;
    * disable Compositor (xcompmgr);
