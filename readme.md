# What is it?

`free_ranger` is an underwater ranging module.  It can detect an
obstacle from about 0.5m to 3m in front of it.

The hardware for `free_ranger` is simple enough to build on a breadboard
with easily-available components, and should cost less than $10.
The circuit as drawn is powered by 2 lithium cells in
series, about 7.8V.

# How to use it.
Build the circuit.

Then see the sample code.  Basically, call `range()`, which will
return a number representing the time of flight of the acoustic
reflection from the object in units of (approx.) hundreds of
microseconds.  `free_ranger` cannot detect objects less than about
0.5m/18" in front of it.  For that, `range()` will return 6 (minimum
distance) or -1 (no reflection).

The call will take about 4ms to complete, and it should be run
with interrupts off, i.e. call `cli()` before and `sei()` after.

To convert to approximate range, multiply the returned value by
7.7cm.  For greater accuracy, you have to measure and calibrate.
However, given the large range resolution (7.7cm), this is of
limited value.

# How it works.
The hardware consists of a transmitter and a separate receiver.
Each has their own transducer.

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

The received signal consists first of transmitter noise, then
transmitter ringing, then a pause, and finally the reflection.

For simplicity, the receiver is always on, so it hears the noise
and ringing but the code ignores them.  Most of the transmitter
noise comes through the rails and is ultimately due to transmitter
current spikes.

Even after the code has shut down the transmit transducer, it will
ring (mechanically oscillate) for about 600us.  This ringing is
picked up by the receiver as a direct signal and can interfere with
the reflection.  This is the reason objects less than 600us distant
(about 18 inches) cannot be discerned: the reflection is
swallowed up by the ringing.

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

The code counts (in `buckets[]`) the number of
comparator "highs" in each 100us (approx.) interval.  Then it
looks for the first run of 8 highs after the ringing phase,
and considers that to be the start of the reflection.  The time
that this occurred is returned as the range value.

Even though the code sends only 2 pulses, ringing will put many more
in flight, and reflections and interference may add to the return
pulse train length.  This is why the code looks for 8 reflected pulses.

The two operations of recording the triggers and searching for runs
are done separately in the code, because recording is done in a
timed loop.

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
