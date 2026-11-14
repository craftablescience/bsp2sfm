from PySide import QtGui


class Dialog(QtGui.QDialog):
	def __init__(self, parent = None):
		QtGui.QDialog.__init__(self, parent)
		line = QtGui.QLineEdit(self)
		label = QtGui.QLabel("hello . I would like to play a little game", self)


filename, _  = QtGui.QFileDialog.getOpenFileName(None, 'Open Map')
if filename:
	dlg = Dialog()
	dlg.exec_()
