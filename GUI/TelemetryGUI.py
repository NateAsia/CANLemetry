import tkinter as tk
from tkinter import ttk
import serial
import os



class LED(tk.Canvas):
    def __init__(self, master, size=20, color="red", off_color="black", **kwargs):
        super().__init__(master, width=size, height=size, **kwargs)
        self.size = size
        self.color = color
        self.off_color = off_color
        self.create_oval(size/4, size /4, size, size, fill="black", outline="black")
        self.state = False
        self.update_state()

    def update_state(self):
        fill_color = self.color if self.state else self.off_color
        self.itemconfig(1, fill=fill_color)

    def set_state(self, state):
        self.state = state
        self.update_state()

class CANLabel(tk.Label):

    def __init__(self, frame=None, base:str ="value:", canId:str ="0x000", canBase:int=0, canLen:int=1, mult:int=1, div:int=1, adder:int=0):
        super().__init__(master=frame, text=base)
        self.base = base
        self.canId = canId
        self.canBase = canBase
        self.canLen = canLen
        self.mult = mult
        self.div = div
        self.adder = adder
        self.value = ""
        # self.update_state()
    
    # def update_state(self, data):
    #     return
    


        

class SerialButtonReader(tk.Tk):
    def __init__(self, port="", baudrate=115200, **kwargs):
        super().__init__(**kwargs)
        self.geometry("600x800")
        self.port = port
        self.baudrate = baudrate
        self.serial_monitor_label = None
        self.connected_led = None
        self.serial_connection = None
        self.setup_ui()

    def setup_ui(self):
        self.title("Telemetry GUI")
        menu_bar = tk.Menu(self)
        menu_bar.add_cascade(label="Select Port", menu=self.create_menu())
        self.config(menu=menu_bar)
        connectedFrame = ttk.Frame(self)
        connect_button = tk.Button(connectedFrame, text="Connect", command=self.setup_serial())
        connect_button.grid(row=0, column=1)
        led = LED(connectedFrame, color="green", off_color="red")
        led.grid(row=0, column=0)
        self.connected_led = led

        # topOuterFrame = ttk.Frame(self)


        # centerFrame = ttk.Frame(topOuterFrame)

        # pb = tk.ttk.Progressbar(
        #     centerFrame, 
        #     orient="vertical", 
        #     maximum=3000, 
        #     value=1500, 
        #     mode="determinate",
        #     length=270
        #     )
        # pb.grid(row=0, column=0, padx=10)

        # pb = tk.ttk.Progressbar(
        #     centerFrame, 
        #     orient="vertical", 
        #     maximum=3000, 
        #     value=1500, 
        #     mode="determinate",
        #     length=270
        #     )
        # pb.grid(row=0, column=2, padx=10)  

        # pb = tk.ttk.Progressbar(
        #     centerFrame, 
        #     orient="vertical", 
        #     maximum=3000, 
        #     value=1500, 
        #     mode="determinate",
        #     length=270
        #     )
        # pb.grid(row=0, column=1, padx=5)  

        serialMonitorLabel = CANLabel(self, base="PLEASE CONNECT TO STEERING WHEEL")
        self.serial_monitor_label = serialMonitorLabel

        connectedFrame.pack(pady=(10,0))
        serialMonitorLabel.pack(pady=(0,15),side=tk.BOTTOM)

    # def customButton(self, name:str ="BTN", color:str ="green", flip=False, row=None, col=None,  frame=None):
    #     if row == None:
    #         row = len(self.leds)
    #     if col == None:
    #         col = 0
    #     if not frame:
    #         frame = self
    #     led = LED(frame, size=100, color=color)
    #     led.grid(row=row,column= (col+1) if flip else col, padx=0, pady=0)
    #     button = tk.Button(frame, text=name, state=tk.DISABLED)
    #     button.grid(row=row, column=col if flip else col+1, padx=0, pady=0)
    #     self.buttons.append(button)
    #     self.leds.append(led)
        

    def set_port(self, port):
        self.port = "/dev/" + port
        self.setup_serial()

    def create_menu(self):
        menu = tk.Menu(self, tearoff=0)
        ports = [p for p in os.listdir('/dev') if p.startswith('cu')]
        for port in ports:
            menu.add_command(label=port, command=lambda p=port: self.set_port(p))
        return menu

    def setup_serial(self):
        if self.port:
            try:
                self.serial_connection = serial.Serial(self.port, self.baudrate)
                self.read_serial()
            except serial.SerialException as e:
                print(f"Error: {e}")
                self.connected_led.set_state(False)
                

    def read_serial(self):
        if self.serial_connection:
            try:
                self.connected_led.set_state(True)
                data = self.serial_connection.readline().decode().strip()
                if data:
                    self.update_ui_status(data)
            except serial.SerialException as e:
                print(f"Error: {e}")
                self.serial_connection = None
        else:
            self.connected_led.set_state(False)
            # self.setup_serial()
            return

        self.after(1, self.read_serial)  # Check for data every 100 milliseconds

    def update_ui_status(self, data):
        self.update_serial_monitor_label(data)
        sections = data.split(':')
        
    
    def update_serial_monitor_label(self, serial_in):
        self.serial_monitor_label.config(text=serial_in)

if __name__ == "__main__":
    app = SerialButtonReader()
    app.mainloop()