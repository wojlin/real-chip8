# CHIP8 Machine


# About:


<img src="3D/perspective.png" width="30%">
<img src="3D/front.png" width="30%">
<br>
<img src="3D/right.png" width="30%">
<img src="3D/top.png" width="30%">

# Documentation:


## How To Build:

0. Parts needed

    |                 Part                   | Amount | Link |
    |----------------------------------------|--------|--------|
    | 5V 4A power supply                     | 1x     | [link](https://botland.store/socket-power-supply/8850-power-supply-5v-4a-dc-55-25-plug-5903351242530.html) |
    | tact switch with cap                   | 16x    | [link](https://botland.store/tact-switch/11138-tact-switch-12x12mm-cap-black-5-pieces-5904422307530.html)
    | micro sd card reader                              | 1x     | [link](https://botland.store/memory-cards-accessories/8230-micro-sd-card-reader-module-5904422311278.html)
    | yellow 3mm led                         | 1x     | [link](https://botland.store/leds/19988-led-3mm-yellow-10pcs-justpi-5904422328764.html)
    | red 3mm led                            | 1x     | [link](https://botland.store/leds/19989-led-3mm-red-10pcs-justpi-5904422328771.html)
    | 64x32px 2.5mm waveshare matrix display | 1x     | [link](https://www.waveshare.com/rgb-matrix-p2.5-64x32.htm)
    | arduino mega                           | 1x     | [link](https://botland.store/arduino-compatible-boards-dfrobot/2685-dfrobot-mega-2560-v32-compatible-with-arduino-6959420922345.html)
    | arduino uno                            | 1x     | [link](https://botland.store/arduino-basic-boards/1060-arduino-uno-rev3-module-a000066-7630049200050.html)
    | 5mm power switch                       | 1x     |     |

    you will also need some wires, soldering tools, polishing papers, clips, screwdriver to asseble this machine

1. Printing & assembly

    - find print files in 3D/stl directory
    - print all the files:

        |        File        | Color | Amount |
        |--------------------|-------|--------|
        | display_cover.stl  | gray  | 1x     |
        | keyboard_cover.stl | gray  | 1x     |
        | screw_2x.stl       | gray  | 2x     |
        | screw_4x.stl       | gray  | 4x     |
        | display_mount.stl  | white | 1x     |
        | keyboard_mount.stl | white | 1x     |
        | keyboard_holder.stl| white | 1x     |
        | bottom.stl         | white | 1x     |
        | bottom_right.stl   | white | 1x     |
        | joiner_3mm_2x.stl  | white | 2x     |
        | joiner_5mm_2x.stl  | white | 2x     |
        | cartridge.stl      | black | min 1  |

        you will need and least:

        - 0.7kg of white filament
        - 0.2kg of gray filament
        - 0.05kg of black filament

    - launch 3D/mode/chip8.blend file using blender to see how it should be assembled
    - assemble parts (you may need to use polishing paper, tools and nail polish remover to make parts fit nicely)
    - apply coat or paint if neeeded
    - to mount bottom with the top parts use 4x mm bolts
    - place 16x 12x12mm tact switches into "keyboard_mount.stl" (use glue to fix them tight)
    - place yellow 3mm led on higher hole into "keyboard_mount.stl" (use glue)
    - place green 3mm led on lower hole into "keyboard_mount.stl" (use glue)
    - insert otg usb cable into cartidge hole into "keyboard_mount.stl" (use glue)
    - place 64x32px rgb matrix display into "display_mount.stl" (use glue to fix it tight)
    - place a usb drive inside of "cartridge.stl"
    - print lables on normal printer and glue then on tact switches caps in this layout:

        | | | | |
        |-|-|-|-|
        |1|2|3|C|
        |4|5|6|D|
        |7|8|9|E|
        |A|0|B|F|


2. Wiring

    - insert arduino mega and arduino uno zero into "bottom.stl" part
    - insert 5v 4A power supply trough first hole in "bottom.stl" part
    - insertpower switch trough second hole in "bottom.stl" part
    - connect tact switches to arduino mega using diagram below:
    - connect leds to arduino mega using diagram below
    - connect display to arduino mega using diagram below
    - connect buzzer to arduino mega using diagram below
    - connect sd card reader to arduino uno using diagram below


    | pin | place | |
    |-|-|-|
    |52 | | |

3. Installing software

    - use your pc to format the usb drive placed in cartidge to FAT extension and make sure that label of that drive is "chip8"(if you want other name you will need to change it inside of runtime/runtime.py file)

    - copy the chip8 roms that you want inside of that usb drive. (you can find rom samples in the samples directory)

    - make sure that only ch8 files are in the drive (remove the trash diretory volumine info etc)

    - clone this repo:

        ```bash
        git clone git@github.com:wojlin/real-chip8.git
        ```

    - download arduino IDE:

        ```bash
        https://www.arduino.cc/en/software
        ```

    - plug machine into 5V 4A socket (!important, do not connect usb cable if power supply is turned off)

    - open sd/sd.ino in arduino IDE

    - upload sd.ino to arduino uno

    - open emulator/emulator.ino in arduino IDE

    - upload emulator/emulator.ino to arduino mega

    - unplug usb cable and mount bottom part of machine with top and secure with screws

    

## Troubleshooting:

1. check if cartidge drive is formated to FAT
2. check if cartridge name is "chip8"
3. check if files are in DOS 8.3 format