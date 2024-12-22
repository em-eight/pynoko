import pynoko

mkw = pynoko.KHostSystem()
mkw.configure(pynoko.Course.Luigi_Circuit, pynoko.Character.Funky_Kong, pynoko.Vehicle.Flame_Runner, False)
mkw.init()
print("MKW initialized")
buttons = pynoko.buttonInput([pynoko.ACCELERATE])
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
