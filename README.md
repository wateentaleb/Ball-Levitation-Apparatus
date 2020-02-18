# Ball Levitation Apparatus

## Objective

The objective is to accurately control the vertical position of the ball inside the Ball Levitation Apparatus. Furthermore, the goal was to were to innovate to create a method or to implement a method to efficiency display data without compromising performance. Various methods were used to tackle the problems, and all will be illustrated and explained in the following report, there will also be a GIF shown below to demonstrate the result. 

## Project Overview 
In this project, the BL 2100 development board is used as the main controller of the system, the board is furthermore interfaced using a serial link to the Development Environment using the Dynamic C IDE to communicate with the apparatus and the demo board as shown in figure 2. Consequently, it is important to note that there are various moving and important parts needed to be used, this feature might be beneficial in decreasing the difficulty of implementing such systems. However, the communication between these devices is vast and will decrease performance due the transmission of data. There is a data lag caused due to the kernel having to spend time on tasks output or receive data while it should be analyzing the actual position of the ball. Furthermore, various strategies as shown in class and in the laboratories were used to increase reliability of this system.

## Project Constraints 
Two demonstration modes have been required in the following project, the first being a variable input which needs to be able to accurately position the desired ball position from the potentiometer already in place in the DEMO board. The second being a step input which is used to set the desired ball position to zero, then half and finally at maximum elevation.
In this implementation, the way that these two modes were switched between each other was with the press of a keyboard key. Pressing the “s” key onto the keyboard switched the system into a step input mode, while pressing the “v” key switched it into the variable input mode. As simple as it seems, this implementation caused problems, questions raised such as how often we should check for user keyboard input, knowing that checking frequently will greatly eradicate performance while not checking enough would cause a miss reading of the user input. Another factor that is important to note is key debouncing, and how to deal with such issues were the first problem tackled in our system. These resolutions will be explained later in the report. Firstly, a demonstration of how each mode was implemented will be discussed.

## Demonstration

![embedded](https://user-images.githubusercontent.com/16707828/74694929-15d3e380-51c0-11ea-88a1-2c25f69f68f2.gif)


## Embedded Positioning System 

A large number of embedded systems involve position control
positioning systems are a sub-class of control systems 
+ Auto focusing mechanism for the lens in a digital camera 
+ Cruise control in a car
Consider the components required to measure a physical quantity, e.g., velocity of a motor used as an actuator

<img width="1166" alt="picture 1" src="https://user-images.githubusercontent.com/16707828/74694752-4ebf8880-51bf-11ea-8b41-d6370c7e1474.png">

## Embedded Control  

From Control Systems point of view the system is modeled as a relationship between its input and output,

<img width="532" alt="Screen Shot 2020-02-17 at 8 01 22 PM" src="https://user-images.githubusercontent.com/16707828/74694982-516ead80-51c0-11ea-9b3c-ecefdef1fde5.png">

If the system is controlled using closed-loop feedback, then

<img width="1171" alt="Screen Shot 2020-02-17 at 8 01 37 PM" src="https://user-images.githubusercontent.com/16707828/74694997-5a5f7f00-51c0-11ea-9d61-2bda0710edc1.png">


## Project Overview 
Using BL 2100 development boards as the main controller. The board is interfaced via a serial link to the Development Environment. While this feature is useful for debugging purposes, care must be made to not interfere with the main functionalities of the embedded system

<img width="1156" alt="overviee" src="https://user-images.githubusercontent.com/16707828/74695030-782ce400-51c0-11ea-88c7-5635cecf95c1.png">


## Tasks Functionalities

Specific functionalities are expected, 
+ PWM signal Generation
+ Based on hardware implementations
+ Position feedback signal – Recall the steps necessary to read an analog signal
(also see next example)
+Calculation of the error signal– Recall the block diagram for a closed-loop control system
+ User Interface for tuning the controller parameters and to set desired output value
+ Logging results such as position of the ball and other data using STDIO window or **other means** without negatively affecting the performance of the board.

## Tasks 

### `Control Task`

 Responsible for calculating a PID-based control signal,
 Should communicate the control signal with PWM generating task or ISR
 
 ### `User Interface Task`

Used for: 
+ adjusting PID gains during run time using pushbuttons or Keyboard.
+ displaying necessary information such as PID gains, **reference point, ball position, and error signal** on computer monitor

 ### `Logging Task`
 
To implement a more efficient communication method with less burden than printf function of Dynamic C , allowing to log the required data more efficiently for generating final report


## Signal Conditioning 

<img width="471" alt="Screen Shot 2020-02-17 at 8 07 29 PM" src="https://user-images.githubusercontent.com/16707828/74695214-2afd4200-51c1-11ea-90ca-ce4e37b02221.png">


For instance to have 0 to 5V output from a pressure sensor which provides -100 to 300mV for 64 to 81cmHg,

<img width="350" alt="Screen Shot 2020-02-17 at 8 07 35 PM" src="https://user-images.githubusercontent.com/16707828/74695234-36e90400-51c1-11ea-9f98-72af92a85e4d.png">


## Infrared Sensor 

An Infra Red (IR) sensor is used to measure the position of the Ball
The output of the sensor is nonlinear and has a limited response time
This information must be considered in the design of position feedback task

<img width="292" alt="Screen Shot 2020-02-17 at 8 08 44 PM" src="https://user-images.githubusercontent.com/16707828/74695263-55e79600-51c1-11ea-9e6e-aa2880de0d7a.png"> <img width="128" alt="Screen Shot 2020-02-17 at 8 08 55 PM" src="https://user-images.githubusercontent.com/16707828/74695292-7a437280-51c1-11ea-83af-eef21c84f2ed.png">


## Nonlinearity

Most transducers have a nonlinear relationship between the physical parameter and the electrical output signal
The relationship is normally given in the form of power series
The coefficients of such series are given for different types and ranges of the device.
The mathematical operations for performing the conversion is an undesirable overhead on the software
The polynomial series are often replaced with a piecewise linear function that replaces each segment of the polynomial with a line segment. For example,

<img width="286" alt="Screen Shot 2020-02-17 at 8 10 52 PM" src="https://user-images.githubusercontent.com/16707828/74695326-9c3cf500-51c1-11ea-8aa3-6966ed1e90f6.png">

<img width="420" alt="Screen Shot 2020-02-17 at 8 11 07 PM" src="https://user-images.githubusercontent.com/16707828/74695337-a4953000-51c1-11ea-891a-86e916cfe967.png">

## THE STEP INPUT MODE

The step input mode was achieved using a PID controller previously taught in Control Systems, a PID controller is an instrument used in control applications to control temperature, flow, pressure, speed and other process variables. The proportional integrative derivative controller is used in a control loop feedback mechanism to control process variables and are the most widely used and accurate controller.

The following is the calculator version of a PID control scheme

<img width="287" alt="Screen Shot 2020-02-17 at 8 23 21 PM" src="https://user-images.githubusercontent.com/16707828/74695821-5aad4980-51c3-11ea-9a82-68ed260d289b.png">
Kp, KI and Kd are proportional, integral and derivative gains, and e(t) is the error signal.


Implementing such a complex mathematical equation on the system will greatly reduce performance due to computation times, a more software-oriented solution was suggested in the form of sums. Which can be easily implemented using recurrence equations which can be implemented using a function that calls itself which changing the parameters. The following equation is shown below, T is the time interval between two samples and kT is the sample time.

<img width="488" alt="Screen Shot 2020-02-17 at 8 24 25 PM" src="https://user-images.githubusercontent.com/16707828/74695864-816b8000-51c3-11ea-8348-a536a8597045.png">

Consequently, using a recurrence equation will also greatly limit system performance and accuracy since the time complexity of such an algorithm will create various calls of the own equation and can also condense the stack depending on the amount of recursive and computational space needed to compute this. In order to further increase the systems performance and decreasing lag, the Ziegler-Nichols Method was implemented.


## ZIEGLER-NICHOLS TUNING METHOD

The Ziegler-Nichols Tuning method applies to plants with neither integrators nor dominant complex-conjugate poles, whose unit-step response resemble a S-shaped curve with no overshoot. This S-shaped curved is known as the reaction curve. The S-shaped reaction curve can be characterized by two constants, being the delay time L and time constant T, which are determined by drawing a tangent line at the inflection point of the curve and finding the intersections of the tangent line with the time axis and the steady-state level line. Finding the above-mentioned variables are visually demonstrated in figure below. The following table was used to find the controllers individually using the L and T variables.

<img width="527" alt="Screen Shot 2020-02-17 at 8 25 23 PM" src="https://user-images.githubusercontent.com/16707828/74695898-a8c24d00-51c3-11ea-8655-9ed063baa802.png">

After using the table, values were calculated. However, this method is not a one stop shop and will not give accurate values of all constants using this method. Tuning needs to be used experimentally. However, this method gives an incredible starting point to where the values should be close to.



