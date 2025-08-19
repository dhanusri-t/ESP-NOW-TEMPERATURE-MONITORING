import serial
import json
import csv
from datetime import datetime

# === Configuration ===
SERIAL_PORT = "COM10"   # change to your central ESP32's port (e.g., COM3 on Windows, /dev/ttyUSB0 on Linux)
BAUD_RATE = 115200
CSV_FILE = "sensor_data.csv"

# === Setup CSV file (write header if new) ===
try:
    with open(CSV_FILE, "x", newline="") as f:  # "x" creates file if not exists
        writer = csv.writer(f)
        writer.writerow(["id", "temperature", "humidity", "timestamp", "datetime"])
except FileExistsError:
    pass  # File already exists → don’t overwrite header

# === Open Serial Port ===
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud...")

while True:
    try:
        line = ser.readline().decode("utf-8").strip()  # read one line
        if not line:
            continue

        # Debug: print raw line from ESP32
        print("RAW:", line)

        # ESP-IDF log prefix handling → extract JSON part only
        if "{" not in line:
            continue
        json_str = line[line.index("{"):]  # extract from first '{'

        # Parse JSON
        data = json.loads(json_str)

        # Add human-readable datetime
        data["datetime"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # Append to CSV
        with open(CSV_FILE, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([data["id"], data["temperature"], data["humidity"], data["timestamp"], data["datetime"]])

        print(f"✅ Saved: {data}")

    except json.JSONDecodeError:
        print("⚠️ Invalid JSON, skipping:", line)
    except KeyboardInterrupt:
        print("\nStopping logging.")
        break
