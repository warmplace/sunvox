sunvox - version with 32bit (floating point) audio engine.
sunvox_lofi - version with 16bit (fixed point 4.12) audio engine for slow devices.

Low FPS? Please follow the instructions below.

sunvox_opengl (with OpenGL):
  * in the raspi-config -> Advanced Options:
    * set GL driver to "full KMS";
    * enable Glamor;

sunvox (without OpenGL):
  * add the "softrender" option to the SunVox config (~/.config/SunVox/sunvox_config.ini);
  * in the raspi-config -> Advanced Options:
    * disable Glamor;
    * disable Compositor (xcompmgr);
