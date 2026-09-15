# HomeKit Momentary Relay

Reference firmware for a HomeKit-controlled momentary relay using an ESP32-C6 and HomeSpan.

The intended use is a low-voltage button circuit where a relay briefly closes the same two contacts a physical push button would close. Examples include hobby projects, lab controls, and user-owned low-voltage door-release/intercom buttons.

## Safety

This is a reference project, not a certified access-control product.

- Use only on systems you own or are explicitly authorized to modify.
- Verify the target circuit with a multimeter before connecting anything.
- Do not connect the relay module or ESP32 board directly to mains voltage.
- Keep the relay contact side isolated from ESP32 `5V`, `3V3`, `GND`, and GPIO pins.
- Put the finished device in an enclosure with strain relief and insulated terminals.
- Keep the original physical button usable when wiring in parallel.

## Hardware

Tested prototype hardware:

- Seeed Studio XIAO ESP32-C6
- Seeed Studio XIAO Grove Shield
- Seeed Studio Grove Relay
- Relay connected to Grove socket `0/1`
- Relay control pin: `D0` / GPIO `0`

Other ESP32 boards and relay modules may work, but you must verify GPIO mapping, relay polarity, power requirements, and contact ratings.

## Behavior

- Appears in Apple Home as a switch named `Momentary Relay`.
- A HomeKit `ON` request starts a 300 ms relay pulse.
- The HomeKit switch state is reset to off after the pulse.
- Consecutive pulses are allowed after each pulse finishes.
- Requests received during an active pulse are ignored.
- HomeKit `OFF` updates received during an active pulse do not cancel or shorten the relay pulse.
- Relay shutoff is enforced by an ESP32 one-shot timer.
- Production builds disable diagnostic logs to avoid USB serial backpressure when no monitor is open.

## Setup

Install Arduino CLI, then initialize the local toolchain:

```sh
make setup-cli
```

Connect the ESP32 board and find its serial port:

```sh
make board-list
```

Build and upload production firmware:

```sh
make upload-homekit-prod PORT=/dev/cu.usbmodemXXXX
```

The shorter aliases use the production profile:

```sh
make compile-homekit
make upload-homekit
```

## Debug Mode

Debug firmware enables HomeSpan CLI and serial diagnostics:

```sh
make upload-homekit-debug PORT=/dev/cu.usbmodemXXXX
make monitor PORT=/dev/cu.usbmodemXXXX
```

Use debug firmware only for configuration and short diagnostics. Flash production firmware again afterward:

```sh
make upload-homekit-prod PORT=/dev/cu.usbmodemXXXX
```

## Documentation

- [WIRING.md](WIRING.md): wiring, relay contact checks, and installation checklist
- [OPERATIONS.md](OPERATIONS.md): setup code, debug mode, recovery, and long-term checks

## License

MIT. See [LICENSE](LICENSE).
