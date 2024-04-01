Credit to my group member!
Bailey Brake, bbrake01@bu.edu

The mytraffic.c kernel module is developed for the BeagleBone Black platform, implementing a traffic light control system.
Features

### Features
- GPIO Control: Utilizes GPIO pins to drive LED lights representing the traffic signals (Red, Yellow, Green).
- Interrupt Handling: Implements interrupt handlers to detect button presses, enabling dynamic control over the traffic light sequence.
- Operational Modes: Supports multiple operational modes, including:
- Normal: Cycles through Green, Yellow, and Red lights in a traditional traffic light sequence.
- FlashRed: Flashes the Red light, indicating a stop signal.
- FlashYellow: Flashes the Yellow light, typically used to signal caution or to indicate the traffic light is out of operation.
- Pedestrian Mode: Incorporates a pedestrian request button. When activated in Normal mode, the system extends the Red light duration to allow pedestrians to cross safely.
- Character Device Interface: Provides a character device interface, allowing user-space applications to interact with the traffic light controller, read its current state, and adjust settings such as operational mode and cycle rate.

##[Video Demo]()
Useful Sources

https://lwn.net/Articles/532714/
https://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html
https://stackoverflow.com/questions/8832298/gpio-access-from-kernel-space
