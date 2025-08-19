#!/usr/bin/env python3
"""
ESP32 → PC CSV Logger
Reads lines from the ESP32 serial port (your central node logs) and appends
temperature values into sensor_data.csv alongside a timestamp.

Usage:
  1) pip install pyserial
  2) Edit PORT below (e.g., "COM3" on Windows, "/dev/ttyUSB0" on Linux, "/dev/cu.usbserial-XXXX" on macOS)
  3) python logger.py
Stop with Ctrl+C.
"""

import sys
import csv
import re
from datetime import datetime

try:
    import serial
except ImportError:
    print("pyserial not installed. Run: pip install pyserial")
    sys.exit(1)

# ------------------ SETTINGS (edit these) ------------------
PORT = "COM10"         # <-- Change this to your ESP32 port
BAUD = 115200         # Must match your ESP32 monitor baudrate
CSV_FILE = "sensor_data.csv"
# -----------------------------------------------------------

# Pattern to capture temperature from a typical log line like:
# "I (71183) CENTRAL_NODE: Received Temperature: 28.00 °C"
TEMP_REGEX = re.compile(r"Received\s+Temperature:\s*([-+]?\d+(?:\.\d+)?)")

def open_serial():
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        return ser
    except Exception as e:
        print(f"Failed to open serial port {PORT}: {e}")
        print("• On Windows: check Device Manager for the correct COM port.")
        print("• On Linux: try /dev/ttyUSB0 or /dev/ttyACM0 (and add your user to 'dialout' group).")
        print("• On macOS: try /dev/cu.usbserial-xxxx or /dev/cu.SLAB_USBtoUART.")
        sys.exit(1)

def main():
    ser = open_serial()

    # Open CSV once; append mode, create if missing
    with open(CSV_FILE, "a", newline="") as fcsv:
        writer = csv.writer(fcsv)
        # Header if file is new/empty
        if fcsv.tell() == 0:
            writer.writerow(["Timestamp", "Temperature_C"])

        print(f"Logging to {CSV_FILE}. Press Ctrl+C to stop.")
        print(f"Reading from {PORT} @ {BAUD} baud...")

        try:
            while True:
                try:
                    line = ser.readline().decode(errors="ignore").strip()
                except KeyboardInterrupt:
                    raise
                except Exception:
                    line = ""

                if not line:
                    continue

                # Try to extract temperature
                m = TEMP_REGEX.search(line)
                if m:
                    temp = m.group(1)
                    ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                    writer.writerow([ts, temp])
                    fcsv.flush()
                    print(f"{ts}  ->  {temp} °C  (saved)")
                # Optional: uncomment to also mirror all serial lines
                # else:
                #     print(line)

        except KeyboardInterrupt:
            print("\nStopped.")
        finally:
            try:
                ser.close()
            except Exception:
                pass

if __name__ == "__main__":
    main()
