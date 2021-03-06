## Status

Hi.  I've stopped working on this project temporarily, because
it has reached the point where it can do what I need from
it, even though it is nowhere near the HC-SR04 level of performance.

I've come to think that it _cannot_ reach that level of
performance at all, for these reasons.

Whereas the ranger performs as expected in a tank, it doesn't
work as well in a pool.  A pool is larger, and the signals
levels are significantly lower that it becomes difficult to
separate it from noise, e.g. from surface reflections.

The 40kHz transducers are operating at much too low of a
frequency to get good directionality from, and so reflections
from the walls or bottom or surface interefere with reflections
from the target.

The ranger is good enough to detect a wall 1.x metres in front of it,
and for the moment that is all that I need.

Also I believe this basic hardware is sufficient to form the
basis of a very simple acoustic modem, so I may work more on
that.

The usual readme follows.

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

When `range_sounding()` is called, the code sends a burst of 2
pulses at 31kHz to Q2, which in conjunction with L1,
can be recognised as a boost converter.  (This used to be a
class C with a tank, but that just prolongs the ringing.)
Peak-to-peak voltage is around 100V.  Be careful if substituting
the transistor, some will cause continuous oscillation.  The
4401 shown is ok.

L1 should be chosen to have a low series resistance and high
current, at least 100mA.

With a 2-series lithium battery, the transducer will see about
100Vp-p across it during oscillation, due to inductor action.

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
resistors R6 and D1 (green) bias the signal at about 1.9V.

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
The total gain of both stages is about 600.

##Detection
In this revision, the method of detection has changed.
Previously, the comparator in the MCU is used to detect when the
signal surpasses a threshold.

Now dithering is employed i.e. that threshold is a
large-amplitude sawtooth generated by the MCU (timer 2 in the
Arduino code) and the code compares the signal against the
sawtooth, sampling every 0.75 microseconds (about as fast as
possible).  The counts are
added up and the result is qualitatively the time series of the
signal envelope.

Sadly this sampling is very much dependent on timing, and
different Arduino IDE versions (and compiler options) will
change it.  So you'll see a bit of assembler in the code.

The sawtooth waveform is generated off the ranger board.  It's
basically a PWM square wave signal of 61.5kHz fed into a low
pass filter composed of a 10k and a 6.8nF cap.

The Arduino interface does not natively provide an API to the
comparator, so the code manipulates the comparator registers
directly.

