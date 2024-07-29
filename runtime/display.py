import platform
import time
import math

if platform.system() == "Linux" and "raspberrypi" in platform.uname().machine:
    from rgbmatrix import RGBMatrix, RGBMatrixOptions, graphics
else:
    from RGBMatrixEmulator import RGBMatrix, RGBMatrixOptions, graphics  # only for mocking


class Display:
    def __init__(self, files):
        self.files = files
        self.selected = False
        self.select = -1

    def launch(self):
        return self.process()

    def process(self):
        options = RGBMatrixOptions()
        options.rows = 32
        options.cols = 64

        self.matrix = RGBMatrix(options = options)

        selected = -1

        try:
            # Start loop
            print("Press CTRL-C to stop sample")
            return self.run()
        except KeyboardInterrupt:
            print("Exiting\n")
            return selected

    def wait_for_selection(self, selected):
        i = input()
        if i == 's':
            return (selected + 1,0) if selected + 1 < len(self.files) else (selected,0)
        elif i == 'w':
            return (selected - 1, 0) if selected - 1 > 0 else (0,0)
        elif i == "q":
            return (selected, 1)
        else:
            return (selected, 0)

    def run(self):
        offscreen_canvas = self.matrix.CreateFrameCanvas()
        font = graphics.Font()
        font.LoadFont("./4x6.bdf")
        textColor = graphics.Color(255, 255, 255)

        amount = 4
        selected = 0

        while True:
            offscreen_canvas.Clear()

            x = 1
            y = 9
            block = math.floor(selected / amount)
            for i in range(amount):
                if i+(block*amount) < len(self.files):
                    text: str = self.files[i+(block*amount)]
                    if len(text) > 12:
                        text = text[:10] + "..."
                    text = text.ljust(13, ' ')
                    if i == selected - (block*amount):
                        text = text + " <"

                    graphics.DrawText(offscreen_canvas, font, x, y, textColor, text)
                    y += 7

            time.sleep(0.05)

            offscreen_canvas = self.matrix.SwapOnVSync(offscreen_canvas)

            data = self.wait_for_selection(selected)
            selected = data[0]
            confirm = data[1]
            if confirm:
                print(f"choosen {selected} options which is {self.files[selected]}")
                self.selected = True
                self.select = selected
                return selected


if __name__ == "__main__":
    filelist = ["tetris", "amogus", "space invaders", 'maze',
                'grid', "other1", "other2", "other3",
                'long text test', 'very long text test', 'very very long test']
    display = Display(filelist)
    print(display.launch())