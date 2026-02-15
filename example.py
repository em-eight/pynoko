import argparse
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
        #print("MKW set input")
        mkw.calc()
        #print("MKW frame stepped")
        print(mkw.kartObjectProxy())
        print(mkw.raceCompletion())
    mkw.reset()
    print("MKW race reset")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run MKW with selected ghost file.")
    parser.add_argument("ghost", help="Path to the .rkg ghost file")
    args = parser.parse_args()
    main(args)
