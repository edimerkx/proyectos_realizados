
import tkinter as tk
from tkinter import ttk
import serial
from openpyxl import Workbook
import time
import os

class ArduinoControlApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Reactor de Aguas")
        self.root.geometry("1280x720")
        self.serial_connection = None
        self.factor_conversion1 = 7.5
        self.factor_conversion2 = 7.5

        self.tiempo = []
        self.v1_data, self.v2_data, self.v3_data, self.v4_data = [], [], [], []
        self.start_time = time.time()

        self.setup_serial()
        self.create_widgets()
        self.create_graphs()

        self.excel_workbook = Workbook()
        self.excel_sheet = self.excel_workbook.active
        self.excel_sheet.title = "Datos Serial"
        self.excel_sheet.append(["Timestamp", "Caudal1","Caudal2","Presion1","Presion2"])
        self.root.config(bg="#F0F8FF")
        self.root.after(10, self.read_serial)

    def setup_serial(self):
        try:
            self.serial_connection = serial.Serial('COM7', 9600, timeout=0.1)
            self.serial_connection.flushInput()
            print("Conexión serial establecida.")
        except Exception as e:
            print(f"No se pudo conectar al puerto serial: {e}")

    def send_command(self, command):
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.write(f"{command}\n".encode())
            print(f"Enviado: {command}")
        else:
            print("No hay conexión serial activa.")

    def create_widgets(self):
        ttk.Button(self.root, text="Activar RELE", command=lambda: self.send_command("RELE_ON")).grid(row=0, column=0, pady=5)
        ttk.Button(self.root, text="Desactivar RELE", command=lambda: self.send_command("RELE_OFF")).grid(row=0, column=1, pady=5)

        ttk.Label(self.root, text="Velocidad (SPEED):").grid(row=1, column=0, pady=5, sticky="e")
        self.speed_slider = ttk.Scale(self.root, from_=1500, to=200, orient="horizontal", command=self.update_speed_label)
        self.speed_slider.grid(row=1, column=1, pady=5, sticky="ew")
        self.speed_label = ttk.Label(self.root, text="200")
        self.speed_label.grid(row=1, column=2, pady=5)
        ttk.Button(self.root, text="Enviar Velocidad", command=self.set_speed).grid(row=1, column=3, pady=5)

        ttk.Label(self.root, text="Factor (Ka):").grid(row=2, column=0, pady=5, sticky="e")
        ttk.Label(self.root, text="#Pulsos/(VOL*60)").grid(row=2, column=2, pady=5, sticky="e")
        self.factora_var = tk.StringVar(value=str(self.factor_conversion1))
        self.factora_entry = ttk.Entry(self.root, textvariable=self.factora_var)
        self.factora_entry.grid(row=2, column=1, pady=5)
        ttk.Button(self.root, text="Enviar Factor", command=self.set_factora).grid(row=2, column=3, pady=5)

        ttk.Label(self.root, text="Factor (Kb):").grid(row=3, column=0, pady=5, sticky="e")
        ttk.Label(self.root, text="#Pulsos/(VOL*60)").grid(row=3, column=2, pady=5, sticky="e")
        self.factorb_var = tk.StringVar(value=str(self.factor_conversion2))
        self.factorb_entry = ttk.Entry(self.root, textvariable=self.factorb_var)
        self.factorb_entry.grid(row=3, column=1, pady=5)
        ttk.Button(self.root, text="Enviar Factor", command=self.set_factorb).grid(row=3, column=3, pady=5)

        ttk.Label(self.root, text="Pausa (PAUSE ms):").grid(row=4, column=0, pady=5, sticky="e")
        self.pause_slider = ttk.Scale(self.root, from_=0, to=5000, orient="horizontal", command=self.update_pause_label)
        self.pause_slider.grid(row=4, column=1, pady=5, sticky="ew")
        self.pause_label = ttk.Label(self.root, text="0 ms")
        self.pause_label.grid(row=4, column=2, pady=5)
        ttk.Button(self.root, text="Enviar Pausa", command=self.set_pause).grid(row=4, column=3, pady=5)

        ttk.Button(self.root, text="Iniciar (START)", command=lambda: self.send_command("START")).grid(row=0, column=2, pady=5)
        ttk.Button(self.root, text="Detener (STOP)", command=lambda: self.send_command("STOP")).grid(row=0, column=3, pady=5)
        ttk.Button(self.root, text="Dirección (DIR)", command=lambda: self.send_command("DIR")).grid(row=0, column=4, pady=5)
        style = ttk.Style()
        style.configure("TButton", font=("Arial", 10), padding=6, background="#B0E0E6", relief="flat")
        ttk.Label(self.root, text="Monitor Serie:").grid(row=0, column=6, pady=5, sticky="w")
        self.serial_output = tk.Text(self.root, height=4, width=80)
        self.serial_output.grid(row=1, column=6, columnspan=4, pady=5)

    def create_graphs(self):
        frame_display = tk.Frame(self.root)
        frame_display.grid(row=6, column=0, rowspan=4, columnspan=10, sticky="nsew", pady=20)

        self.valor_caudal1 = tk.StringVar()
        self.valor_caudal2 = tk.StringVar()
        self.valor_presion1 = tk.StringVar()
        self.valor_presion2 = tk.StringVar()

        ttk.Label(frame_display, text="Caudal 1 (L/min):", font=("Arial", 14)).grid(row=0, column=0, sticky="e")
        self.lbl_caudal1 = ttk.Label(frame_display, textvariable=self.valor_caudal1, font=("Arial", 14))
        self.lbl_caudal1.grid(row=0, column=1, sticky="w")
        self.pb_caudal1 = ttk.Progressbar(frame_display, orient="horizontal", length=300, maximum=30)
        self.pb_caudal1.grid(row=0, column=2, padx=10)

        ttk.Label(frame_display, text="Caudal 2 (L/min):", font=("Arial", 14)).grid(row=1, column=0, sticky="e")
        self.lbl_caudal2 = ttk.Label(frame_display, textvariable=self.valor_caudal2, font=("Arial", 14))
        self.lbl_caudal2.grid(row=1, column=1, sticky="w")
        self.pb_caudal2 = ttk.Progressbar(frame_display, orient="horizontal", length=300, maximum=30)
        self.pb_caudal2.grid(row=1, column=2, padx=10)

        ttk.Label(frame_display, text="Presión 1 (bar):", font=("Arial", 14)).grid(row=2, column=0, sticky="e")
        self.lbl_presion1 = ttk.Label(frame_display, textvariable=self.valor_presion1, font=("Arial", 14))
        self.lbl_presion1.grid(row=2, column=1, sticky="w")

        ttk.Label(frame_display, text="Presión 2 (bar):", font=("Arial", 14)).grid(row=3, column=0, sticky="e")
        self.lbl_presion2 = ttk.Label(frame_display, textvariable=self.valor_presion2, font=("Arial", 14))
        self.lbl_presion2.grid(row=3, column=1, sticky="w")

    def read_serial(self):
        if self.serial_connection and self.serial_connection.is_open:
            try:
                line = self.serial_connection.readline().decode().strip()
                if line:
                    self.process_serial_data(line)
                    self.update_serial_output(line)
            except Exception as e:
                print(f"Error en lectura serial: {e}")
        self.root.after(10, self.read_serial)

    def process_serial_data(self, line):
        if line.count(",") == 3:
            try:
                v1, v2, v3, v4 = map(float, line.split(","))
                t = round(time.time() - self.start_time, 1)
                self.tiempo.append(t)
                self.v1_data.append(v1)
                self.v2_data.append(v2)
                self.v3_data.append(v3)
                self.v4_data.append(v4)
                self.excel_sheet.append([t, v1, v2, v3, v4])
                if len(self.tiempo) > 100:
                    self.tiempo.pop(0)
                    self.v1_data.pop(0)
                    self.v2_data.pop(0)
                    self.v3_data.pop(0)
                    self.v4_data.pop(0)

                self.valor_caudal1.set(f"{v1:.2f}")
                self.valor_caudal2.set(f"{v2:.2f}")
                self.valor_presion1.set(f"{v3:.2f}")
                self.valor_presion2.set(f"{v4:.2f}")
                self.pb_caudal1["value"] = v1
                self.pb_caudal2["value"] = v2
                self.lbl_caudal1.config(foreground="red" if v1 > 20 else "black")
                self.lbl_caudal2.config(foreground="red" if v2 > 20 else "black")

            except ValueError:
                pass

    def update_speed_label(self, value):
        self.speed_label.config(text=f"{int(float(value))}")

    def update_pause_label(self, value):
        self.pause_label.config(text=f"{int(float(value))} ms")

    def set_speed(self):
        speed = int(self.speed_slider.get())
        self.send_command(f"SPEED:{speed}")

    def set_factora(self):
        factora = self.factora_var.get()
        try:
            float(factora)
            self.send_command(f"Ka:{factora}")
        except ValueError:
            print("Factor de conversión inválido.")

    def set_factorb(self):
        factorb = self.factorb_var.get()
        try:
            float(factorb)
            self.send_command(f"Ka:{factorb}")
        except ValueError:
            print("Factor de conversión inválido.")

    def set_pause(self):
        pause = int(self.pause_slider.get())
        self.send_command(f"PAUSE:{pause}")

    def update_serial_output(self, data):
        self.root.after(0, lambda: self.insert_data(data))

    def insert_data(self, data):
        self.serial_output.insert(tk.END, data + "\n")
        self.serial_output.see(tk.END)

    def on_closing(self):
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
        save_path = os.path.join(os.path.expanduser("~"), "Documents", "datos_serial.xlsx")
        try:
            self.excel_workbook.save(save_path)
            print(f"Datos guardados en '{save_path}'")
        except Exception as e:
            print(f"Error al guardar el archivo Excel: {e}")
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = ArduinoControlApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
