import board
import busio

from kmk.kmk_keyboard import KMKKeyboard
from kmk.scanners.keypad import KeysScanner
from kmk.keys import KC
from kmk.modules.layers import Layers
from kmk.modules.macros import Macros, Press, Release, Tap

from kmk.extensions.display import Display, DisplayText
from kmk.extensions.display.ssd1306 import SSD1306


keyboard = KMKKeyboard()

#EXTENSIONS
layers = Layers()
keyboard.modules.append(layers)

macros = Macros()
keyboard.modules.append(macros)

#OLED SDA=GPIO0,SCL=GPIO7
i2c = busio.I2C(scl=board.GP7, sda=board.GP0)
oled_text = DisplayText()
oled = Display(
    display=SSD1306(
        i2c=i2c,
        device_address=0x3C,
        width=128,
        height=32,
    ),
    display_text=oled_text
)
keyboard.extensions.append(oled)

#SWITCHES
PINS = [
    board.GP4,   # SW1
    board.GP3,   # SW2
    board.GP2,   # SW3
    board.GP1,   # SW4
    board.GP27,  # SW5
    board.GP26,  # SW6
    board.GP28,  # SW7
    board.GP29,  # SW8
    board.GP6,   # SW9
]

keyboard.matrix = KeysScanner(
    pins=PINS,
    value_when_pressed=False,
)

#LAYERS, MAKRA
keyboard.keymap = [

    #Layer 0, Fusion 360
    [
        KC.S,         # SW1
        KC.E,         # SW2
        KC.F,         # SW3
        KC.I,         # SW4
        KC.R,         # SW5
        KC.T,         # SW6

        KC.Macro(Press(KC.LCTL), Tap(KC.Z), Release(KC.LCTL)),  # Undo
        KC.Macro(Press(KC.LCTL), Tap(KC.Y), Release(KC.LCTL)),  # Redo
        KC.MO(1),     # Switch to Photoshop
    ],

    #Layer 1, Photoshop
    [
        KC.B,         # SW1 Brush
        KC.E,         # SW2 Eraser
        KC.V,         # SW3 Move
        KC.Z,         # SW4 Zoom
        KC.H,         # SW5 Hand tool

        KC.Macro(     # New Layer
            Press(KC.LCTL), Press(KC.LSFT), Tap(KC.N),
            Release(KC.LSFT), Release(KC.LCTL)
        ),

        KC.Macro(Press(KC.LCTL), Tap(KC.Z), Release(KC.LCTL)),  # Undo
        KC.Macro(Press(KC.LCTL), Press(KC.LALT), Tap(KC.Z),
                 Release(KC.LALT), Release(KC.LCTL)),            # Step Backward

        KC.MO(0),     # Back to Fusion
    ]
]

#OLED UPDATE
def before_matrix_scan():
    """Toto sa volá automaticky pri každom cykle KMK."""
    if keyboard.active_layers[0] == 0:
        oled_text.text = "Mode:\nFusion 360"
    else:
        oled_text.text = "Mode:\nPhotoshop"

keyboard.before_matrix_scan.append(before_matrix_scan)

#START
if __name__ == '__main__':
    keyboard.go()
