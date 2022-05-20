# RedButton-Project
This is the WIP code for an interactive installment to promote political awareness to youths. 
This was programmed in the Arduino IDE but made compatible with the ESP32 microcontroller and intended to be paired with 
Mini Thermal Receipt Printer : ID 597 although the printing parameters can be changed for other thermal printers.
The thermal printer code is referenced from: https://github.com/BinaryWorlds/ThermalPrinter 

An operator is intended to remote into an android phone or raspberry pi or pc nearby to pilot the device using a serial cli terminal.

User Experience / Interaction loop:
- User sees a table with a big red button
- User presses button
- A receipt is printed through a slot on the table asking them a scripted question
- They reply with a yes or no by pressing the button once or twice.
- They get a scripted reply or a QR code to redirect them the organization website.

- If they are okay with speaking then more open ended questions are asked and they respond through a microphone 
-  The operator can then type witty remarks to their answers
-  Eventually they get to ask a question which the operator would then answer.
-  After the interaction is over a QR code would be printed to redirect towards a landing page where addtional information is collected and the organization is introduced to the users.

Goals of Project:
- Investigate whether a chatbot is worth building
- Is it possible to engage youth, stimulate social media, and get students involved in local politics
- Get youths to talk to their families about local politics
- Collect contact information
