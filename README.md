# GDExtension OSC

GDExtension OSC is a Godot Extension (think like plug-in) that enables reception and emission of [OSC](http://opensoundcontrol.org/introduction-osc?) messages in [Godot game engine](https://godotengine.org/).

The code will compile into a dynamic library, easy to load and use in godot projects.

* GDExtension version of a GDnative version of https://github.com/djiamnot/gdosc.

## about OSC

Open Sound Control (OSC) is a network protocol commonly used in digital arts, stage and live multimedia performances to exchange information between softwares. Based on UDP protocol, it ensures fast communication and nearly no knowledge about network management.

This addon is based on the cross-platorm OSC lib [oscpack](http://www.rossbencina.com/code/oscpack).
[OSC official website](http://opensoundcontrol.org)

## TODO :

- [X] adapt to GD Extension
- [X] Use IWYU
- [ ] Try to move to C++ 20
- [ ] OSCPack dependancy update
- [X] Build with nix on linux
- [ ] Build with Github Action
- [ ] Test with Demo
  - [X] GDExtension detectable
  - [X] Class Loaded and detected
  - [ ] Send OSC
  - [ ] Receive OSC

## contributors

- Perard-Gayot Noé [MadMcCrow]
- François Zajéga [frankiezafe](https://gitlab.com/frankiezafe/)
- Michal Seta - [djiamnot](https://gitlab.com/djiamnot/)
- Bastiaan Olij - [mux213@gmail.com](mux213@gmail.com) - special thanks for dgnative c++ template!
