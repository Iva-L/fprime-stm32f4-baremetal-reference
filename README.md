# F´ Stm32 Baremetal Reference Project (STM32F4)

This project is a reference F´ deployment for the STM32F401RE (NUCLEO-F401RE:
Cortex-M4F, 512 KiB flash, 96 KiB SRAM). `Stm32f4Deployment` contains only the
F´ components needed to demonstrate a bare-metal flight-software architecture
within that budget: static initialization, a cyclic executive, cooperative
active components, and a bounded communication path. It is the STM32F4
counterpart of the STM32H7 reference deployment.

F´ (F Prime) is a component-driven framework for developing and deploying
spaceflight and other embedded software applications. See the
[F´ website](https://fprime.jpl.nasa.gov).

`lib/fprime-stm32` provides the reusable OSAL delegates, drivers and bootstrap
allocator. Everything CubeMX-generated or board-specific (this board's `.ioc`
output, clock and tick-source glue, the linker script) stays owned by this
project in `Stm32f4Reference/Hardware/`. The linker script, startup file and
`Hardware/CMakeLists.txt` are produced from the CubeMX project by
`fprime-stm32 sync`; see the
[CLI README](lib/fprime-stm32/tools/fprime-stm32-cli/README.md).

## Board configuration

| Item | Setting |
| --- | --- |
| System clock | 84 MHz from the PLL, fed by the 8 MHz ST-LINK MCO (HSE bypass) |
| Ground link | USART2 on PA2/PA3 (ST-LINK virtual COM port), 115200 8N1, DMA1 streams 6 (TX) and 5 (RX) |
| Time base | TIM2 at 1 MHz (prescaler 83); channel 2 compare drives the 10 ms rate-group tick |
| LED | PA5, driven through `Stm32::Stm32GpioDriver` |
| Debug | SWD (PA13/PA14) |

The clock values live in `Hardware/config/src/stm32f4_clock.c` and target this
board. Review them before reusing the project on another F4 board.

## Configuring which hardware ports are enabled

Every STM32 driver reads which peripheral instance it may use from
`Stm32Config.hpp`. This project overrides the library default with its own copy
at `Stm32f4Reference/config/fprime-stm32/Stm32Config.hpp` (via `settings.ini`'s
`config_directory`); only `USART2` is enabled. When adding a peripheral, enable
it in that copy as well as in CubeMX. Selecting a disabled instance in a
driver's `open()` call fails a `FW_ASSERT`.

## Build

The project uses the GNU Arm Embedded toolchain and the `stm32f4` F´ platform
with `MinSizeRel`. From the project root:

```shell
source fprime-venv/bin/activate
fprime-util generate -f stm32f4
fprime-util build stm32f4
```

The ELF is written to
`build-artifacts/stm32f4/Stm32f4Deployment/bin/Stm32f4Deployment` and the
dictionary to
`build-artifacts/stm32f4/Stm32f4Deployment/dict/Stm32f4DeploymentTopologyDictionary.json`.

## Stm32f4Deployment

The deployment runs without threads or an operating system. Its main loop:

1. Initializes the HAL, the 84 MHz clock tree and the TIM2 time base.
2. Configures the generated topology and locks the bootstrap allocator.
3. Calls `Os::Baremetal::TaskRunner::runAll()` to execute one cooperative
   dispatch step for every registered active component.
4. Polls the TIM2 tick source and the UART driver.

The topology imports `CdhCore` and `ComCcsds` and adds two rate groups (1 Hz and
0.25 Hz, divisors 100 and 400 of the 10 ms tick), `Svc::OsTime`, `STM32Timer`,
`Stm32UartDriver` and `Stm32GpioDriver`. The GPIO driver is opened on PA5 but no
component toggles it yet.

Deliberately left out to fit 96 KiB:

- File handling, data products, command sequencer and `SystemResources`.
  `SystemResources` asserted on the first 1 Hz cycle because it reads a
  filesystem that MicroFs never initialized here.
- MicroFs. Components that need persistent files cannot be added until it is
  initialized.

`config/` carries a reduced framework configuration (queue depths, telemetry
hash tables, command tables) sized for this component set.

## Static memory contract

The deployment avoids dynamic allocation after initialization.
`Os_Baremetal_OverrideNewDelete` routes C++ allocations through a fixed 32 KiB
bootstrap pool (`FPRIME_STM32_BOOTSTRAP_POOL_SIZE` in
`lib/fprime-stm32/cmake/platform/stm32f4.cmake`). The startup file registers it
before the C++ constructors run, and `Stm32::lockBootstrapAllocator()` freezes
it after setup, so an allocation during cyclic execution asserts.

The F4 has a single, contiguous, DMA-visible SRAM, so unlike the H7 reference
there is no AXI SRAM/DTCM split. The linker script reserves a 4 KiB stack (plus
512 B of heap) at the top of RAM, and `_sstack` marks its bottom.

| Region | Capacity |
| --- | ---: |
| `FLASH` | 512 KiB |
| `RAM` | 96 KiB |

Measured with `arm-none-eabi-size` on the current build:

| Region | Used | Remaining |
| --- | ---: | ---: |
| Flash (`.text` + `.data`) | 206,476 bytes | 60.6% |
| RAM `.data` + `.bss` | 84,104 bytes | |
| RAM stack/heap reserve | 4,608 bytes | |
| RAM total | 88,712 bytes | 9.8% (9,592 bytes) |

The bootstrap-pool usage is a runtime figure and has not been measured on
hardware yet.

## Validation status

This project was flashed to aNUCLEO-F401RE and ran under the debugger 
for 40+ seconds with breakpoints on`HardFault_Handler`, `Fw::Logger::log` 
(assertions) and `Svc::FatalHandler::FatalReceive_handler`; none was hit. 
After the run, TIM2 had counted about 4,760 ticks, `rateGroup_1Hz` had executed 48 cycles, 
and the timer overrun counter was 0.

To check the ground link, run the GDS against the board's virtual COM port:

```shell
fprime-gds --dictionary build-artifacts/stm32f4/Stm32f4Deployment/dict/Stm32f4DeploymentTopologyDictionary.json \
  --communication-selection uart --uart-device /dev/ttyACM0 --uart-baud 115200 --no-app
```