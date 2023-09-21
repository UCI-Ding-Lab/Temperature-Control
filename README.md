# Ding Temperature Control
An accurate, customizable, and cost-effective temperature control system
Please reach out to [UCI Ding Lab](https://www.ding.eng.uci.edu) for any questions or requests. 

The temperature control system is designed to maintain a stable temperature from room temperature to 60 °C within 0.06 °C. On the aluminum target block, two adhesive polyimide heating elements and two temperature sensors are attached on the side. These sensors and heating elements are connected to an Arduino Nano which controls the temperature output with an external 24V power supply using the PID algorithm. 

This GitHub repository includes the PCB KiCad and Gerber files, PCB enclosure STL files, and Arduino code.

### Major steps to build the system.
![Protocol Graphical Abstract](https://github.com/UCI-Ding-Lab/Temperature-Control/assets/18509861/f62a3cb3-b496-48d2-9e7a-cf232f32378a)

## PCB
The PCB is designed in KiCad 6.0, in which users can edit the layout for free. It is a 2-layer board with all through-hole components. 
The top layer is a +5V plane and the back layer is a ground plane. 
It houses the Arduino Nano, female headers, DC barrel jack, and screw terminal jack.

### Layout of the PCB
<img width="467" alt="Ding Temperature Control PCB Layout" src="https://github.com/UCI-Ding-Lab/Temperature-Control/assets/18509861/c2fbfb6e-abaf-4f37-87ea-2d1a9d269fc5">

### KiCad model of the PCB
![Ding Temperature Control PCB Transparent](https://github.com/UCI-Ding-Lab/Temperature-Control/assets/18509861/287069da-e2cb-484a-8604-a7f40e08ee17)

## Enclosure
The enclosure is designed in Fusion 360 with a top and bottom piece. It is designed to be printed on a 3D printer with PLA and without any support. 
It contains M3 threaded inserts to connect the PCB, LCD, and buck converter. 

A Fusion 360 model of the enclosure.
![Ding Temperature Control Housing](https://github.com/UCI-Ding-Lab/Temperature-Control/assets/18509861/8f1781f5-2730-48d2-a911-ddc5892c7245)

## Arduino Code
The Arduino Nano on the PCB reads the temperature sensors and calculates the heating power to reach the target temperature with the PID algorithm. 
Then, a PWM signal is sent to the MOSFET with the heating elements to heat up the target block.
