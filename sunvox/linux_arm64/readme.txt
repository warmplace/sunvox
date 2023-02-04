Minimum system requirements for this version:
Linux (arm64); ARMv8-A (Cortex-A53); glibc 2.28 (2018)


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
