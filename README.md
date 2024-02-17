PicoSync is an open source device designed to use a Raspberry Pi Pico (RP2040) to fix a common C-Sync issue, and improve compatibility between devices and monitors. In its barebones state of just the Pi Pico board, is only compatible with TTL Csync. Compatibility with 75ohm, H/V sync, and sync on Composite/Luma/Green could be added through simple external hardware.

The Problem:

XNOR combination logic is one of the most common methods of combining Hsync and Vsync into Csync. It is utilized on many sync combiners (VGA to RGBS converters), and internally on some older game systems such as the Sega Master System and PC engine. When Horizontal Sync (H-Sync) and Vertical Sync (V-Sync) are combined using an XNOR logic, one of he horizontal pulses during the vertical blanking period is lost. This missing falling edge can cause issues on some high end monitors, such as the BVM-D9. Common symptoms of this issue include inability to sync, or significant flagging/warping at the top of the screen. For more further explination of this issue, see HD Retrovisions's "Engineering Csync Part 2" here: https://www.hdretrovision.com/blog/2019/10/10/engineering-csync-part-2-falling-short


How it works:

This code utilizes the Pi Pico's PIO to search for the missing falling edge, and inject a falling edge if one is missing. If the falling edge is present, the signal is passed through unaffected. 


Pinout:

Pin 1: C-Sync In
Pin 2: C-Sync Out
Pin 3: GND


Testing:

Testing was performed using a Time Sleuth to generate various HDMI resolutions, an HDMI to VGA DAC, a double XOR sync combiner (effectively an XNOR gate), and a BVM-D9. The sync fix has been tested to work with 240p, 480i, 480p, 1080i, 540p, and 720p. 


Sync Voltage Level:

-Pico Sync does not require any additional components, but without additional hardware, it is limited only to TTL level C-Sync for both input and output. You may want to use logic level conversions, as the Pi Pico operates at 3.3v logic. 


H/V Sync:

-While initially Pico Sync was compatible with both H/V signals and Csync, support for separate sync was dropped. Performing the logical sync combination within software was not as stable or fast as using a hardware logic gate. Using an XNOR gate (or 2 XOR gates) requires only using 1-2 parts that cost pennies each. Performing combination with hardware before the PicoSync results in the the same functionality (compatibility with both combined and separate sync, c sync through hsync input, no need to toggle) and improves performancce. 


Sync on Composite/Luma/Green:

-Pico Sync does not have direct compatibility with Sync on Composite/Luma/Green. To add this functionality, you will need to integrate a sync extraction circuit, such as one based on the LM1881. This would als provide compatibility with both 75ohm and TTL level csync. This circuit could also be combined with a XNOR sync combiner for near universal sync compatibility.


Credits:

Adewotta - Writing the entirety of the Pico Sync code
Shank - Diagnosing the issue, pitching solution, hardware testing


Additional thanks:

HD Retrovision - Excellent documentation of C-Sync issues through their aformentioned blog
RadicalPlants / Trevor Rudolph - Initial brainstorming/information


