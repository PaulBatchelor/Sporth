import("effect.lib");

drive = hslider("[0]drive", 0, 0, 1, 0.0001);
offset = hslider("[1]offset", 0, 0, 4, 0.0001);

process = cubicnl(drive, offset) : dcblocker;
