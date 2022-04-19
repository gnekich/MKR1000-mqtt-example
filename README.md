# Arduino MQTT Project

A simple example of Arduino MKR1000 subscribing to ~~shiftr.io~~ Adafruit IO MQTT broker and turning on/off LEDs and Relay

This is school project for members of KIOK (Klub Informatičara Otoka Krka)

For more info or suggestions regarding this project, you can reach to the authors.

# Note

After few days of using Shiftr.io stopped working, so we moved to Adafruit.io which worked perfectly.
Shiftr.io has a limit of 6h per instance under a free subscription.

We had to do a few changes to support Adafruit.io feeds, MQTT topics had to be prefixed with `kiok/feeds/<feed_name>` instead of `/<feed_name>`

## Authors

- Gordan Nekić <gordan@neki.ch>
- Boris Bolšec <boris.bolsec2@ri.t-com.hr>
