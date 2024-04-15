# main.py

import sys
from PyQt5.QtWidgets import QApplication
from creatingwindow import MyGUI
import hostmachine

if __name__ == "__main__":
    # Launch GUI
    app = QApplication(sys.argv)
    gui = MyGUI()
    gui.show()

    # Launch host machine communication
    hostmachine.run_communication()

    sys.exit(app.exec_())
