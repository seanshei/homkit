# Wiring

## Intended Circuit

This project is intended for low-voltage button circuits where pressing a physical button briefly shorts two terminals together.

The relay contacts should be wired in parallel with the existing button:

- One relay screw terminal to one side of the existing button pair.
- The other relay screw terminal to the other side of the existing button pair.
- Keep the original physical button connected.

This lets either the original button or the relay close the same circuit.

## Relay Contact Check

Before connecting to the target circuit, use a multimeter in continuity mode across the relay screw terminals:

- Idle relay: open circuit.
- Active relay: continuity / near 0 ohms.

If your relay module has `COM`, `NO`, and `NC` markings, use `COM` and `NO`. Do not use `NC` for momentary button emulation unless you have a specific reason and understand the fail state.

## Target Circuit Check

Before wiring:

- Measure the button terminals with a multimeter.
- Confirm the voltage/current are within the relay contact rating.
- Confirm the button action is a simple momentary closure.
- Stop if the circuit behavior is unclear.

The ESP32 and Grove side must remain electrically separate from the relay contact side.

## Safety Checklist

Before connecting:

- Unplug USB power from the ESP32.
- Disconnect the target device connector if available.
- Confirm the relay is not powered while handling bare wires.
- Confirm there are no loose wire strands at screw terminals.
- Insulate all exposed conductor.
- Add strain relief so cable tension cannot pull on the screw terminals.
- Mount the board so it cannot touch metal or conductive surfaces.
- Use a stable 5 V USB power adapter and a short, reliable USB cable.

After connecting:

- Test the original physical button.
- Test one Home app trigger.
- Test several Home app triggers spaced about one second apart.
- Confirm the relay only clicks briefly for each trigger.
- Power-cycle the ESP32 and confirm the relay does not trigger on boot.

## Long-Term Installation Notes

- Use an enclosure before permanent installation.
- Avoid locations with moisture, dust, vibration, or exposed metal.
- Avoid routing relay wires next to sharp edges or moving parts.
- Leave access to USB for future firmware updates.
- Label wiring so it can be disconnected safely later.
