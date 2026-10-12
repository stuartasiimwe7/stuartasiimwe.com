# Room logger

Status: design and firmware scaffold only. No hardware purchased, assembled or tested in this task. No measured accuracy, uptime or power claims.

## Build choice

Costs below are planning allowances excluding reusable instruments, shipping and tax, not quotations for complete kits.

| Build | Cost allowance | Learning | Portfolio evidence | Debugging scope |
|---|---|---|---|---|
| ESP32 + SHT31 room logger (selected) | $36–50 | I²C, sensor reads, timing, serial acquisition | Raw data, repeatable tests, fault records | Missing sensor, invalid reads, self-heating, resets |
| Soil-moisture indicator | $20–40 | ADC, voltage ranges, threshold hysteresis | Useful only with soil-specific calibration | Probe variability, corrosion, drift |
| Low-voltage fan controller | $35–65 | MOSFET drive, PWM, supply design | Strong control-system demonstration | Startup current, noise, heating; more hardware complexity |

The logger has the best first-build balance. Start with USB serial rather than a dashboard. Wi-Fi publishing is a later extension after the baseline is measured. This is a temperature/humidity logger, not an air-quality or CO₂ monitor.

## Exact baseline bill of materials

Listings checked 2026-10-06; price/availability may change. Quantities are one each unless specified.

| Part | Specification / purchase link | Listed price |
|---|---|---|
| Controller | Adafruit ESP32 Feather V2 **with headers**, PID 5900, USB-C | $20.95 |
| Sensor | Adafruit SHT31-D STEMMA QT breakout, PID 2857 | $13.95 |
| Sensor cable | Adafruit JST-SH 4-pin QT-to-QT 100 mm, PID 4210 | $0.95 |
| USB cable | USB-A or USB-C host to USB-C **data** cable, 1 m | Reuse; otherwise budget $5–10 |
| Mounting | Two nylon M2 fasteners/standoffs for sensor; nonconductive base, leave sensor ventilated | Budget $3–5; confirm hole fit before ordering |

Core subtotal: **$35.85**, excluding delivery and taxes. No soldering iron, breadboard, battery, OLED or relay needed for this baseline.

- https://www.adafruit.com/product/5900
- https://www.adafruit.com/product/2857
- https://www.adafruit.com/product/4210

Reusable test equipment: digital multimeter with DC volts and continuity (budget ₩20,000–40,000, allowance only); independent temperature/RH reference (borrow first, document model and stated uncertainty). Optional 3.3 V-compatible USB logic analyzer for I²C waveforms; not required for first acquisition.

Korean purchasing alternatives, **not drop-in replacements**: DeviceMart ESP32-DEVKITC-32E (₩18,238 VAT included) and DFRobot SEN0331 SHT31 (₩15,840 VAT included; listing says roughly one week preparation, overseas/order item). These need jumper wiring, appropriate USB cable, different board selection and pin definitions. Do not mix their wiring with the Feather firmware unchanged.

- https://www.devicemart.co.kr/goods/view?no=14033167
- https://www.devicemart.co.kr/goods/view?no=12543556

## Wiring

Disconnect USB before connecting or removing the sensor. Plug the 100 mm cable between Feather QT and sensor QT. Leave sensor ADDR at default 0x44. Do not feed 5 V into ESP32 signal pins. Use USB as the only power source; no battery or mains wiring.

| QT wire | Feather connection | Sensor connection |
|---|---|---|
| Black | GND | GND |
| Red | Switched 3.3 V QT supply | VIN |
| Blue | SDA (board-defined Wire SDA) | SDA |
| Yellow | SCL (board-defined Wire SCL) | SCL |

Firmware explicitly enables NEOPIXEL_I2C_POWER. Sensor breakout includes I²C pullups; Feather does not. Keep the supplied short cable. Mount sensor away from the ESP32 regulator and airflow obstruction; record separation in photos. Do not claim calibration from agreement with a consumer hygrometer.

Official wiring reference: https://learn.adafruit.com/adafruit-esp32-feather-v2/pinouts
I²C diagnostics: https://learn.adafruit.com/adafruit-esp32-feather-v2/i2c-scan-test

## Firmware and repository

- `firmware/room_logger/room_logger.ino`: Arduino bring-up, I²C scan, 5-second CSV acquisition, explicit error state and sensor reinitialization.
- `tools/capture.py`: USB serial capture with host UTC timestamp and raw board uptime. Requires pyserial; invalid values remain empty.
- `evidence/measurements.csv`: header only; populate with actual instrument observations.
- Store real photos, firmware/core/library versions, raw captures and debug notes under `evidence/` after testing. Never commit Wi-Fi credentials or identifiable home-network details.

In Arduino IDE install Espressif ESP32 board support, select **Adafruit Feather ESP32 V2**, install **Adafruit SHT31 Library** and its dependency **Adafruit BusIO**, and open the sketch. Record exact installed versions and build output in evidence before claiming reproducibility. Compile/upload are still unverified here.

Capture: `python -m pip install pyserial`, then `python tools/capture.py /dev/ttyACM0 evidence/run.csv`. Use the actual enumerated port (may be ttyUSB0). Opening serial may reset the board. Each capture records a host timestamp; board uptime is not wall-clock time.

## Test milestones and acceptance targets

All entries are **pending**, and all numbers below are targets rather than results.

1. **Power:** inspect unpowered connections, then measure QT red-to-black DC voltage with insulated probes. Target 3.3 V ±5%; record actual voltage and meter model. Do not short adjacent pins or use continuity on powered hardware.
2. **Bus bring-up:** serial at 115200; scan identifies 0x44; boot with sensor absent reports missing rather than fabricated values. If absent: check data cable, QT enable, ground, cable orientation and sensor supply. Capture serial logs.
3. **Acquisition:** collect 10 minutes at 5-second cadence (target approximately 120 records); verify values are finite and RH is within 0–100%. Count valid/invalid samples and maximum interval from raw logs.
4. **Comparison:** co-locate reference and sensor for 30 minutes, then record 10 paired readings one minute apart. Report mean difference, range and reference uncertainty. Do not use this as a certified accuracy measurement or silently offset firmware values.
5. **Fault tests:** power off, remove sensor, reboot; expect error records and empty values. Power off, restore sensor, reboot; expect reacquisition. Runtime bus error recovery is a firmware target requiring separate validation; do not hot-plug I²C to demonstrate it. Reset board during capture and preserve uptime discontinuity.
6. **Soak:** 24-hour capture (target 17,280 samples); report actual duration, valid fraction, resets and largest gap. Check sensor placement effects with separated vs adjacent mounting in otherwise similar conditions.
7. **Publish evidence:** replace pending states only after photos, wiring, raw data, plots and troubleshooting records exist. Document any failed targets. Add enclosure and Wi-Fi only as a subsequent version.

## Website structure

Card: name, planned/hardware-untested status, ESP32/SHT31/I²C tags and link to build page. No stock image presented as an assembled device.

Page: purpose → status → parts → wiring → firmware → test milestones → evidence/results → sources. Add actual build photo, annotated wiring and charts once obtained. Each result needs a capture date, hardware revision, software versions and raw-data link. A planned project must remain visibly distinct from completed work.
