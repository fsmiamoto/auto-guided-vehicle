# auto-guided-vehicle

Final project for the Embedded Systems class of [Universidade Tecnológica Federal do Paraná](https://utfpr.edu.br).

The objective is to develop an RTOS based system that is able to guide a simulated vehicle that receives commands through an UART interface.

Developed using:
- [Texas Instruments Tiva C Series TM4C1294 LaunchPad](https://www.ti.com/lit/ml/spmz858/spmz858.pdf)
- [IAR Embedded Workbench 9](https://www.iar.com/ewarm)
- [Keil RTX5](https://www2.keil.com/mdk5/cmsis/rtx)
- [SimSE 2 Simulator](https://pessoal.dainf.ct.utfpr.edu.br/douglasrenaux/index_files/Page392.htm) (portuguese)

> Dedicated to our greatly missed Professor Hugo Vieira.
His lessons and dedication will always be remembered.

<img src="./simulator/screenshot.png" alt="Screenshot of simulator running" width=500 />

## Code

Most of the source code developed is at the [vehicle-controller](https://github.com/fsmiamoto/auto-guided-vehicle/tree/master/src/Projects/vehicle-controller)
directory which contains the IAR 9 project files.

## Architecture

![](./docs/img/architecture.png)

