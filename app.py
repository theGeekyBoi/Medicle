from csv import Dialect
import sys
from PyQt5.QtWidgets import (
    QApplication,
    QLabel,
    QMainWindow,
    QStatusBar,
    QToolBar,
    QPushButton,
    QWidget,
    QDialog
)
from PyQt5.QtGui import QPixmap
import app_ui
import pres_ui
import random
import serial

# change this to your serial port
# ser = serial.Serial('COM3', 9600)
# data = ser.readline()

# data = data.split(',')

# put these values in the respective function to get it
# realtime from the arduino
# height = data[0]
# weight = data[1]
# bmi = (weight/(height*height))*10000
# temp = data[3]
# hr = data[4]
# spo2 = data[5]
# bp_sys = data[6]
# bp_dia = data[7]



class Window(app_ui.Ui_Form):
    def __init__(self):
        self.mainwin = QWidget()
        self.setupUi(self.mainwin)
        self.mainwin.setWindowTitle("Doctor App")

        # self.my_btn1 = QPushButton("BMI",self)
        # self.my_btn2 = QPushButton("Heart Rate",self)
        # self.my_btn3 = QPushButton("Blood Pressure",self)
        # self.my_btn4 = QPushButton("Temperature",self)

        #self.btn_bmi.clicked.connect(self.get_BMI)
        #self.btn_bp.clicked.connect(self.get_BP)
        #self.btn_hr.clicked.connect(self.get_HeartRate)
        #self.btn_temp.clicked.connect(self.get_Temp)
        self.btn_pres.clicked.connect(self.get_pres)
        #self.ph_height.clicked.connect(self.get_height)
        #self.pb_weight.clicked.connect(self.get_weight)
        #self.pb_spo2.clicked.connect(self.get_spo2)


        self.mainwin.show()

    def get_height(self):
        temp = round(random.uniform(150.0,180.0),2)
        self.lb_height.setText(str(temp)+'cm')

    def get_weight(self):
        temp = round(random.uniform(50.0,90.0),2)
        self.lb_weight.setText(str(temp)+'kg')

    def get_BMI(self):
        temp = round(random.uniform(18.5,24.9),2)
        self.lb_BMI.setText(str(temp))
    
    def get_BP(self):
        # 90/60mmHg and 120/80
        temp1 = random.randint(60,90)
        temp2 = random.randint(80,120)
        self.lb_BP.setText(str(temp1)+'/'+str(temp2)+'mmHg')
    
    def get_HeartRate(self):
        temp = random.randint(60,100)
        self.lb_HR.setText(str(temp)+'BPM')
    
    def get_Temp(self):
        temp = round(random.uniform(36.1,37.2),2)
        self.lb_temp.setText(str(temp)+'°C')
    
    def get_spo2(self):
        temp = random.randint(95,100)
        self.lb_spo2.setText(str(temp)+'%')

    # doctor prescription
    def get_pres(self):
        screen = QApplication.primaryScreen()
        screenshot = screen.grabWindow(self.mainwin.winId() )
        screenshot.save('patient_discription.jpg', 'JPG')

        self.dia = QDialog(self.mainwin)
        pres_dialog = pres_ui.Ui_Form()
        pres_dialog.setupUi(self.dia)
        self.dia.setWindowTitle("Prescription")

        pres_dialog.label.setText(self.le_doc_name.text())
        pres_dialog.label_2.setText(self.le_patient.text())
        
        pres_dialog.pushButton.clicked.connect(self.pb_print)

        self.dia.exec()
    
    def pb_print(self):
        screen = QApplication.primaryScreen()
        screenshot = screen.grabWindow(self.dia.winId() )
        screenshot.save('C:/Users/91981/Downloads/doc_project (2)/doc_project/doctor_priscription.jpg', 'JPG')


 
if __name__ == "__main__":
    app = QApplication([])
    window = Window()
    sys.exit(app.exec())