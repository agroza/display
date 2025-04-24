# display

Text-Mode File Viewer Program

## Synopsis

While working on the [documentation](https://github.com/agroza/aif/blob/master/output/MANUAL.TXT) for the [AIF](https://github.com/agroza/aif) hardware driver, I quickly realized that the ```AIF``` software could be deployed on systems that might not have any text-mode file viewer available. Popular file viewers were ```EDIT.COM``` or the embedded file viewer of the TUI file managers like Dos Navigator or Norton Commander. Such streamlined systems are most certainly rarer these days. But back in the early 1990s, it was common to see streamlined MS-DOS versions, without the DOS directory containing all the additional programs, drivers, and tools distributed by Microsoft. Well, technically, these streamlined systems contained at least the Volkov Commander as file manager.

But anyway, you get the point. And it was not uncommon for software packages of the '80s and '90s to come with their own small text-mode file viewer. For example, Borland Pascal 7.0 (and possible earlier versions) came with ```README.COM```. In fact, during the mid- to late-1990s, while programming DiskInfo, I coined a small ```.COM``` readme-like utility. But that had the documentation self-contained. While simpler to develop, it was actually hard to maintain.

By the end of the 1990s, I abandoned the text-mode file viewer idea, as I was into Windows programming for the next two decades.

## Motivation

However, with my recent interest in retrocomputing and MS-DOS software development, I realized that I have to revisit the text-mode file viewer idea. Thus, ```DISPLAY.COM``` was born, written entirely in x86 assembly language. The compiled program is around 900 bytes (to be exact, 892 bytes for V. 1.2), and displays any text file that is passed as commandline parameter. However, the file should not exceed 62 Kb, otherwise it will be truncated to the first 62 Kb. I even used ```DISPLAY.COM``` for assembly source code viewing as it quickly proved useful in more ways than I initially planned.

In addition, I wrote an official documentation that goes as [DISPLAY.TXT](https://github.com/agroza/display/blob/master/output/DISPLAY.TXT).

The source code is small and self-explanatory. I tried to keep things simple and reduced any unneeded overhead. I also had to minify my VersaVision MS-DOS video driver in order to keep the compiled file size at a minimum. For the same reasons, I tried to optimize the code as much as I could. Additionally, error handling and signaling was reduced to the bare minimum necessary.

It's funny that the text on this page weighs around 3.7 Kb, while ```DISPLAY.COM``` is less than 1 Kb. Even ```DISPLAY.TXT``` occupies around 2.7 Kb of drive space.
The size of assembly language computer programs always puzzled me back in those days. And even though I now have a better understanding of how computers work, they continue to do so to this day.

### Versions

The first version (which I didn't bother to upload to GitHub) used BIOS interrupts for rendering the file contents on the screen. But that was slow and caused flickering. Thus, I programmed the second version, that uses the video RAM directly.

#### Future Ideas

At some point, I might think about improving the program to read files of any size, as long as they can fit into the available conventional memory.
But the chaos of memory pagination and tracking ```SEGMENT:OFFSET``` pairs, is really not appealing to me. Needless to say, that would increase the compiled program size considerably.

### Program Usage

The following line is taken directly from the commandline help screen, if ```DISPLAY.COM``` is executed without any commandline parameter.

```
Use: display.com <file.ext>
```
