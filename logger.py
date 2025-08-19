import serial
import json
import csv
from datetime import datetime

# ----------------------------
# USER CONFIGURATION
# ----------------------------
COM_PORT = "COM10"              # <--- Change this if needed
BAUD_RATE = 115200
CSV_FILE = "sensor_data.csv"    # Logs will be appended here
# ----------------------------

# Open serial connection
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
print(f"Listening on {COM_PORT} at {BAUD_RATE} baud...")

# Open CSV in append mode
with open(CSV_FILE, "a", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=["id", "temperature", "humidity", "datetime"])

    # If file is empty, write the header first
    if f.tell() == 0:
        writer.writeheader()

    try:
        while True:
            line = ser.readline().decode("utf-8").strip()
            if not line:
                continue

            print("RAW:", line)

            if line.startswith("Raw Data:"):
                try:
                    data = json.loads(line.split("Raw Data:")[1].strip())

                    # Drop "timestamp" if present
                    data.pop("timestamp", None)

                    # Round values
                    if "temperature" in data:
                        data["temperature"] = round(float(data["temperature"]), 2)
                    if "humidity" in data:
                        data["humidity"] = round(float(data["humidity"]), 2)

                    # Add datetime from PC
                    data["datetime"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                    # Write to CSV
                    writer.writerow(data)
                    f.flush()
                    print(f"Saved: {data}")

                except json.JSONDecodeError:
                    print("Invalid JSON, skipping...")
    except KeyboardInterrupt:
        print("\nLogging stopped by user.")
