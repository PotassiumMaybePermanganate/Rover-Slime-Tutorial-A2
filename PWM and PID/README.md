# PWM-PID Module

## PWM

The PWM module uses timer A SMCLK to generate the pulse width required to move the DC motors. It comes with variable speed when moving forward. The default speed is 75% duty cycle at 4500. There are two more speed configuration at 50% duty cycle and 40% duty cycle.

## PID
PID controller stands for proportional, integral and derivative controller. It calculates previous error against a target set point and derives the optimal pulse width to output.

![pid diagram](./assets/pid_diagram_ticks.png)
![flowchart](./assets/pwm_pid_flowchart.png)
![pid duty output mapping](./assets/pid_output_mapping_graph.jpeg)
![whitebox testing](./assets/whitebox_console_output.jpeg)
