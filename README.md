Medicle, a play on words, is a culmination of the words Medical and Cubicle because that is precisely what it is, a Medical Cubicle. Every and any diagnosis requires a few vitals to be tested. These essential metrics are -
Height,
Weight,
Body Mass Index,
Temperature,
Blood Pressure,
Oxygen Saturation / SPO2,
Heart Rate,

Medicle constitutes a cubicle with an array of sensors that will conduct all these indispensable tests automatically. The crucial data collected will then be transferred over the web to a firebase hosted server, wherein the doctor can access the information from anywhere with an internet connection.

The doctor then, armed with both the vital details of a patient and a face-to-face discussion over a zoom call, will make his judgment and deliver his diagnosis. The doctor may suggest a medication to be administered, might identify a solution to the problem the patient faces or might clear any concerns the patient might have. The doctor might also recommend that the patient travel to a hospital for more detailed tests and further examination if deemed necessary.

Medicle, therefore, saves the patient both time, expenditure, and effort in getting a diagnosis, only having the patient travel to a hospital if absolutely necessary. The patient also rapidly gets a diagnosis as the doctor receives the vital data promptly through automated tests.

Medicle runs on the Arduino Mega and is coded in C++. The 313 lines of code ensure that the sensor automation is accurate, rapid, and precise. Our own algorithm takes hundreds of values over a short period of time and averages them out to compute highly accurate data. 

We’ve used 4 libraries in the program -
HX711
Wire
MAX30105
spo2_algorithm

The program composition includes functions for each component which are accessed through character inputs from the Serial Monitor. The doctor will remotely control these character inputs and guide the patient through the automated tests. Once the data is received the doctor will access the GUI that we built using the python library PyQt5. Through this, the doctor types out his prescription/recommendation and prints it out remotely for the patient to take with them.
