Lab 2 Writeup
=============

My name: [your name here]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about [n] hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
[]

Implementation Challenges:
[]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

``` cpp
void TCPReceiver::segment_received(const TCPSegment &seg);
```

Consider 16 cases:

||_syn = true<br>_fin = true|_syn = true<br>_fin = false|_syn = false<br>_fin = true|_syn = false<br>_fin = false|
|-|-|-|-|-|
|S|ignore|ignore|
|F|||ignore|ignore|
|SF|ignore|ignore|ignore|ignore|
|other|||ignore|ignore|