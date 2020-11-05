This folder contains curve files for such modules as: MultiSynth, WaveShaper, MultiCtl.
You can create your own curves using the sunvox_curve_generator.pixi - this is the source code of the program, written in Pixilang programming language.
How to use the generator:
 1) open sunvox_curve_generator.pixi in your favorite text editor like a regular text file;
 2) change the SETUP section:
    filename = "file name for your curve";
    format =
      0 - MultiSynth curve1;
      1 - MultiSynth curve2;
      2 - MultiSynth curve3;
      4 - WaveShaper;
      5 - MultiCtl.
    curve_function() - here you can place you own function for the specified curve;
 3) download Pixilang - https://warmplace.ru/soft/pixilang
 4) launch Pixilang, choose sunvox_curve_generator.pixi for execution;
 5) the new curve file will be created in the same folder.
