import argparse
import cv2
import pynoko

def main(args):
    mkw = pynoko.KHostSystem()
    mkw.configureTimeTrial(pynoko.Course.Bowsers_Castle, pynoko.Character.Funky_Kong, pynoko.Vehicle.Flame_Runner, False)
    mkw.configureGhost(args.ghost)
    mkw.init()
    print("MKW initialized")
    buttons = pynoko.buttonInput([pynoko.KPAD_BUTTON_A])
    print(f"buttons: {buttons}")

    for i in range(30000):
        mkw.setInput(buttons, 7, 0, pynoko.Trick.NoTrick)
        mkw.calc()
        print(mkw.kartObjectProxy())
        print(mkw.raceCompletion())

        # the rendered frame is a packed RGB float image (origin at the bottom-left);
        # flip vertically and reverse channels to BGR for OpenCV
        cv2.imshow("mkw", mkw.getFrame()[::-1, :, ::-1])
        cv2.waitKey(1)
    mkw.reset()
    print("MKW race reset")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run MKW with selected ghost file.")
    parser.add_argument("ghost", help="Path to the .rkg ghost file")
    args = parser.parse_args()
    main(args)
