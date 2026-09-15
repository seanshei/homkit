# Security

This project controls a relay and may be adapted for sensitive physical actions. Treat any deployed device as part of your local security boundary.

## Recommendations

- Keep the repository free of Wi-Fi credentials, HomeKit setup codes, QR payloads, IP addresses, and device identifiers.
- Change the HomeSpan default setup code before real use.
- Do not expose the ESP32 directly to the public internet.
- Use Apple Home sharing permissions intentionally.
- Keep debug firmware off deployed devices.
- Prefer physical access for recovery and firmware updates.

## Reporting

If you find a safety or security issue in this reference project, open a GitHub issue with enough detail to reproduce the problem, but do not include private credentials or access codes.
