# What is it?

`free_ranger` is an underwater ranging module.  It can detect an
obstacle from about 0.5m to 3m in front of it.

The hardware for `free_ranger` is simple enough to build on a breadboard
with easily-available components, and should cost less than $10.
The circuit as drawn is powered by 2 lithium cells in
series, about 7.4V.

# How to use it.
Build the circuit.  Hook it up to an arduino.

Bring up the `range_demo` code in the Arduino IDE.  Compile and
program.  The code returns the range to the target in
centimetres, or some
large number if out of range.  `free_ranger` cannot detect
targets too close by, the reflection from any object close than
about 0.8m is swamped by the transmission noise.  The reported
range does not go lower than this value for this reason.

# How it works.
The hardware consists of a transmitter and a separate receiver.
Each has their own identical transducer.

## Transmission
40kHz piezoelectric parking sensors are used for the transmit
and receive transducers.  These are already waterproof and are
easily available e.g. from
[ebay](http://www.ebay.com/itm/22-5mm-Black-New-Parking-Sensor-Car-Reverse-Back-Up-Ultrasonic-Radar-L5YG-/361226329538?pt=LH_DefaultDomain_0&hash=item541ac45dc2)
or
[aliexpress](http://www.aliexpress.com/item/Wholesale-4pcs-Assistance-Reversing-Radar-Rrobe-Parking-Sensors-black-blue-gray-red-white-silver/1861230727.html).

When `range()` is called, the code sends 2-40kHz square wave pulses to
T6, the base bias resistor.  This results in transistor Q2
energising the tank circuit L1/C6, which starts oscillating.
The transmit transducer which is connected across this tank at J2,
also oscillates at 40kHz.

L1 should be chosen to have a low series resistance and high
current, at least 100mA.  The tank capacitance is C6 plus about 2nF,
which is roughly the capacitance of the transducer.

With a 2-series lithium battery, the transducer will see about
40Vp-p across it during oscillation, due to inductor action.
This is actually quite a low driving voltage.  These transducers take
probably a couple of hundred volts when used as
parking sensors, but 40V is enough for this purpose.

You can see in [this diagram](trace-no-transducers.bmp) the
ringing in the transmitted waveform (yellow trace).  Similarly,
the received waveform (blue) shows electrical interference from
the transmitter.  [This trace](trace-in-air.bmp) is with the
transducers connected in air, so there is some acoustic
interference as well.

## Reception
A second identical transducer is used for reception.  This is
normally situated next to the transmit transducer, pointing in
the same direction.  But for bench testing, you can point both
transducers at each other a few inches apart (in air).

The signal goes first through an MFB bandpass filter with gain.
This is the first part of the LM358 jellybean op amp.  As shown,
resistors R6 and D1 bias the signal at about 1.6V.

`free_ranger` can be run without a filter if you want to simplify it,
but the filter is cheap insurance against noise and spurs.

The received signal consists first of direct noise from the
transmitter, then transmitter ringing, then a pause, and finally
the reflection, as can be seen [here](trace-tank-length.bmp).
Ringing stops after about 600us, after which there is nothing until
around 3.5ms where the comparator is repeatedly triggered (yellow
and blue traces cross).

For simplicity, the receiver is always on, so it hears the noise
and ringing but the code ignores them.  Some of the transmitter
noise comes through the rails and is ultimately due to transmitter
current spikes.

After filtering, the signal then goes through the gain stage.
The total gain of both stages is about 100.

The comparator in the MCU is used to detect the reflected pulse.
A comparator's output is high/low if the non-inverting input (at
J4:2) is higher/lower than the inverting input (at J4:1).
This behaviour is used
to detect the acoustic reflection, but the technique is a bit
more sophisticated than threshhold detection.
The network R5/C5/R8 messes with the phase,
amplitude, and offset such that only large enough signals of the
right frequency will trigger the comparator.  If all is well, this
happens every 25us when the reflection arrives.

The Arduino interface does not natively provide an API to the
comparator, so the code manipulates the comparator registers
directly.

The code counts (in `cells[]`) the number of
comparator "highs" in each 160us interval.  Then it
looks for the first run of 8 highs after the ringing phase,
and considers that to be the start of the reflection.  The time
that this occurred is returned as the range value.

Even though the code sends only 2 pulses, ringing will put many more
in flight, and reflections and interference may add to the return
pulse train length.  This is why the code looks for 8 reflected pulses.

[This image](trace-pipe-length.bmp) shows the return from a
1-1/2" air-filled PVC pipe placed 0.8m in front of the
transducers.  The reflection around 1ms is due to the pipe, and
the reflection at 3.5ms is due to the far end of the test tank.

# Caveats
You should feel free to fiddle with the code, but be very
careful not to keep Q2 on for too long.  The code uses 12us,
which is 180 degrees of conduction, and there is no great reason
to increase the conduction angle, or you risk burning up Q2.

# Troubleshooting
## Verify wiring
Goes without saying.  Make sure you've wired it up correctly.
- Check transducer polarity: for the transmitter, the coax braid
should go to Vcc.  For the receiver, the coax braid goes to
ground.
- Verify the comparator inputs are not reversed.
- Verify transistor pinout, if you're building on a breadboard.
- Verify op amp pinout, especially inverting and non-inverting
nodes.

## With a multimeter...
- Vcc should be about 7.8V.
- Quiescent current draw (without MCU) should be a couple or
three milliamps.  With an Arduino, it should be around 25mA.
- `vhalf` should be around 1.7V for a red LED.
- J4:1 should be around 2.5V with no signal.
- J4:2 should be 0.1V less than J4:1 with no signal.
- DC offset between `comp-` and `comp+` should be around 75mV

## With a scope.
Set to trigger at T6 on a rising edge.
You should see the two 5V square wave pulses on T6, and
2 transmit pulses (they might look a bit distorted) on J2:2, and
clean ringing, even if the transducer is not connected.
Peak-to-peak voltage should be at least 30V with a
7.8V supply.

With the two transducers facing each other on the bench 6 inches
apart, you should be able to see the signal appearing at J1:2
after 0.5ms, and there should be a dozen or so cycles of about
0.5Vp-p amplitude.

See also the included scope traces.

# Thanks
If you build one, I would be happy to hear
from you: htarold@gmail.com

Thanks.
