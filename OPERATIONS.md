# Operations

## Production Firmware

Use the production profile for normal operation:

```sh
make upload-homekit-prod PORT=/dev/cu.usbmodemXXXX
```

Production behavior:

- Starts `Serial` to keep the ESP32-C6 USB runtime stable.
- Disables HomeSpan serial input.
- Disables HomeSpan serial logs.
- Keeps relay output controlled by the 300 ms one-shot timer.

## Debug Firmware

Use debug firmware only while connected to a computer with Serial Monitor open:

```sh
make upload-homekit-debug PORT=/dev/cu.usbmodemXXXX
make monitor PORT=/dev/cu.usbmodemXXXX
```

Debug mode enables HomeSpan CLI and relay timing logs. Do not leave debug firmware installed for normal use, because serial output can cause HomeKit delays when no monitor is draining the USB serial buffer.

## Wi-Fi Setup

HomeSpan stores Wi-Fi credentials in ESP32 non-volatile storage.

Typical setup flow:

1. Flash debug firmware.
2. Open Serial Monitor.
3. Send HomeSpan CLI command `W`.
4. Follow the prompts to enter Wi-Fi credentials.
5. Flash production firmware after pairing and testing.

Do not put Wi-Fi credentials in source files.

## HomeKit Setup Code

HomeSpan recommends setting the setup code through CLI or setup AP instead of hardcoding it in firmware.

Changing the setup code does not normally require removing an already-paired accessory from Apple Home. It affects future pairing attempts.

Steps:

1. Pick a private 8-digit numeric code with no dashes.
2. Flash debug firmware:

```sh
make upload-homekit-debug PORT=/dev/cu.usbmodemXXXX
make monitor PORT=/dev/cu.usbmodemXXXX
```

3. In Serial Monitor, send:

```text
S 12345678
```

Replace `12345678` with the private code.

4. Record the code somewhere safe. HomeSpan stores only a hash and cannot display the code later.
5. Flash production firmware again:

```sh
make upload-homekit-prod PORT=/dev/cu.usbmodemXXXX
```

6. Confirm the existing Home app tile still controls the relay.

## Recovery

If Apple Home shows `No Response`:

- Wait 30-60 seconds after upload or power cycle.
- Force quit and reopen the Home app.
- Confirm the device still responds on the network if a computer is available:

```sh
ping -c 3 <device-ip>
nc -vz <device-ip> 80
dns-sd -B _hap._tcp local
```

If a diagnostic session is needed, flash debug firmware and open monitor before testing.

## Long-Term Checks

- Confirm the relay clicks briefly and never stays on.
- Check that the board recovers after power loss.
- Keep the USB power supply stable and accessible.
- Inspect wiring and strain relief after the first few days of real use.

## Do Not

- Do not run normal production use with debug logs enabled.
- Do not hardcode Wi-Fi credentials or HomeKit setup codes in source control.
- Do not erase HomeSpan NVS unless you are prepared to reconfigure Wi-Fi and pairing state.
