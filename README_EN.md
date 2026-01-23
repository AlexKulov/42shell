#42shell - An Extension to 42 for Rapid Startup, Development, and Testing of Onboard Control Algorithms

42shell is based on the original 42 project, which is included as a submodule.  
Key advantages of the 42 project are:  
1) It is written in a single "foundational" programming language, C.  
2) It offers extensive simulation capabilities: orbital and attitude dynamics,  
interplanetary transfers, and simultaneous multi‑spacecraft operations.  
For more details, visit: https://github.com/ericstoneking/42

## Structure of repo
```
-- 42shell
    |-- 42
    |   |-- Contains a origin 42 repo
    |-- 42change
    |   |-- Contains a change 42 code files like 42 strucure
    |-- 42support
    |   |-- Contains freeglut and glew library
    |   |-- Contains code files (QtSource) only Qt lib (Qt5 m.b. more)
    |   |-- Contains qt project file for build from ../42 folder
    |-- shInOut
    |   |-- Like 42/InOut, but demo example for 42shell repo
    |-- 42add
    |   |-- Specific programm models for shell42, but not work without orig 42
	|-- img
    |   |-- just picture for readme
```

## Prerequisites

