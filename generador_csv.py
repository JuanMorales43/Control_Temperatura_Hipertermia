import serial
import csv
import time
arduino_port = "COM3"
baud_rate = 115200
file_name = "pid.csv"
ser = serial.Serial(arduino_port, baud_rate)
with open(file_name, mode="w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(["Tiempo (s)", "Temperatura Máxima (°C)", "Control Output (%)"])
    print("Guardando datos en", file_name)
    try:
        while True:
            line = ser.readline().decode("utf-8").strip()
            print("Datos recibidos:", line)
            if line.startswith("Tiempo"):
                try:
                    parts = line.split(" ")
                    tiempo = float(parts[1])
                    temperatura = float(parts[3])
                    control_output = float(parts[-1])
                    writer.writerow([tiempo, temperatura, control_output])
                except (ValueError, IndexError):
                    print("Error al leer los datos")
                    continue
    except KeyboardInterrupt:
        print("Cerrando archivo...")
    finally:
        ser.close()
