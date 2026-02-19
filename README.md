# 42shell - An Extension to 42 for Rapid Startup, Development, and Testing of Onboard Control Algorithms

42shell is based on the original 42 project, which is included as a submodule.  
Key advantages of the 42 project are:  
1) It is written in a single "foundational" programming language, C.  
2) It offers extensive simulation capabilities: orbital and attitude dynamics,  
interplanetary transfers, and simultaneous multi‑spacecraft operations.  
For more details, visit: https://github.com/ericstoneking/42

## Structure of repo
```
-- 42shell
    |-- 42dev
    |   |-- New programm module for 42 environment: models, fsw and other kit algs
    |-- 42change
    |   |-- Contains a modify 42 code files
    |-- 42core
    |   |-- Contains a lightweight fork 42 repo
    |-- 42support
    |   |-- Contains freeglut and glew library
    |   |-- Contains code files (QtSource) only Qt lib (Qt5 m.b. more)
    |   |-- Contains Qmake (with qplot) and Cmake files for build 42
    |-- examples
    |   |-- Like 42/InOut or 42/Demo, but demo examples for 42shell repo
	|-- img
    |   |-- just picture for readme
```

## Prerequisites  
  
1) Download repo to your computer with submodule 42core  

a) via the command line (requires git installation):  
git clone --recursive https://github.com/AlexKulov/42shell.git   
This command will also load all other submodules.   
  
b) via the website. Download the repo archive
![Figure 1](img/DownLoadRepo.png)  
unzip archive to the desired directory. Folder 42core will be empty.   
To download the contents of folder 42core, follow the submodule link
![Figure 3](img/DownLoadRepo2.png)  
Download it as well     
Now you just need to unzip the resulting archive 42core to the appropriate folder [42core](img/DownLoadRepo1.png)  
    
с)  combined option:  
git clone https://github.com/AlexKulov/42shell.git   
In this case, the 42core folder is empty. Proceed as in b)
**Note** that the [commit hash matches](img/DownLoadRepo3.png)
  
2) Install Qt on your computer (I used Qt5.12).  
**Windows OS Only**: When installing Qt5 (maybe Qt6),  
be sure to add the compiler MinGW 32(64)-bit  
**Linux OS Only (Ubuntu 18)**: Install Qt  
  
## Quick Start for Windows/Linux  
  
**Windows OS Only!!!**: Copy the DLL files from the 42support/glew and 42support/freeglut folders to the 42 folder  
(to run the .exe file compiled in this folder)  
**Windows/Linux (Ubuntu 18)**:  
1) Open the 42support/42.pro file in Qt Creator, select the MinGW 32-bit compiler  
(this is important, as the DLL files are compiled with this in mind) and click "More Details." Next, select the 42support directory  
for all three cases and click "Configure Project."  
** Important! The path to 42support should be latinic characters. **  
![Figure 4](img/ConfigProject.jpg)  
2) Run the project (green triangle with a bug in the lower left)  
3) Click the "Start" button on the widget that appears  
  
3D graphics (42) should appear, and graphs (Qt) should also begin rendering.  
**Linux OS Only (Ubuntu 18)**: likely works on other Linux distributions as well.  
  
## 42InOut Test Cases...

First of all, to work with 42, you need input data (ID) files.  
When 42 starts, it looks for ID files in the ./InOut folder by default.  
During a quick start, this was exactly how it was: the ID files were located in the 42core/InOut folder  
(or simply ./InOut, since the executable file is located in the 42core folder).  
42shell has a folder with its own examples - examples.   
Some of them:  
- **4Mode**: Enables 4 test modes: 3 single-axis (Sun, nadir, and target on Earth)  
and 1 three-axis - orientation in LVLH (in Russian terminology, this is similar to  
orbital coordinate system). The mode switching logic is written in the Inp_Cmd.txt file:  

The spacecraft first aligns its b3 axis with the Sun. After entering the umbra and losing the Sun, the b3 axis  
reorients to the nadir (this occurs approximately over the Atlantic). Upon exiting the umbra into the penumbra,  
the orientation of the b1 and b2 axes is adjusted: b1 is aligned with the flight direction, b2 is perpendicular to the orbital plane  
(this is the LVLH orientation). Then, while flying over Australia, the spacecraft aims b3 at the city of Jayapura  
(Indonesia) with coordinates of 140.71 degrees E and 2.5 degrees S (hardcoded);  
- **Simplest**: the minimum possible set of ID files: spacecraft file, orbit file, simulation file, and command file.  
  
### How do I select the ID folder?  
**From the console**: If you're doing this from the console, simply enter the address of the new ID folder as the first parameter.  
  
For example, ../42InOutSimplest.  
  
**From Qt**:  
1) Open the Projects tab (green key icon on the left)  
2) In the "Build and Run" section, select "Run" (also on the left side of the window)  
3) Find the input line labeled "Command Line Parameters"  
4) Enter ../examples/Simplest in the command line  
  
![Figure 5](img/QtInOutChoice.png)
  
Now you can run the program (again, the green button with the bug) and click "Start."  
  
### Acknowledgments  
Some of the add42 functions (for example, in the fswMode.c file) were created (2024-2025) with the support of the Russian Science Foundation (RSF) grant No. 24-29-00706.  