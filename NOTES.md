# Zephyr Workspace Notes

## Existing Zephyr Installation

Use the existing Zephyr installation for Zephyr projects instead of creating a
separate west workspace for each project.

- Tools and virtual environment: `C:\zephyr`
- West workspace: `C:\zephyr\zephyrproject`
- Zephyr checkout: `C:\zephyr\zephyrproject\zephyr`
- STM32 toolchain and STM32CubeProgrammer: installed through STM32CubeCLT

Projects can live outside the shared west workspace. Set `ZEPHYR_BASE` so CMake
can locate the Zephyr checkout.

## Prepare the Environment

```powershell
. C:\zephyr\setup_env.ps1
$env:ZEPHYR_BASE = "C:\zephyr\zephyrproject\zephyr"
$env:Path = "C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin;$env:Path"
```

Run builds from the west workspace root:

```powershell
cd C:\zephyr\zephyrproject
```

## Build a Project

Replace the placeholders with the target board, a unique build directory, and
the path to the Zephyr application:

west build `
    -b <board> `
    -d build\<build-name> `
    -p auto `
    <application-path>
```

For the LED blinky example on a Nucleo-F446RE:

```powershell
west build `
    -b nucleo_f446re `
    -d build\blinky-nucleo-f446re `
    -p auto `
    <blinky-project-path>
```

## Flash a Project

Use the runner appropriate for the target board and debug probe. For an STM32
board flashed through STM32CubeProgrammer, the `STM32_Programmer_CLI.exe`
directory must be available on `PATH`:

```powershell
west flash `
    -d build\blinky-nucleo-f446re `
    -r stm32cubeprogrammer
```

## Serial Output

When the board exposes a serial console through the debug probe, connect to the
ST-LINK virtual COM port at `115200` baud, 8 data bits, no parity, and 1 stop
bit. The application determines what output appears on the console.
