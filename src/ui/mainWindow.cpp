#include "ui/mainWindow.moc"

#include <iostream>


typedef seamcarve::ui::MainWindow MW;

void MW::displayAboutMessageBox() {
   std::cout << "ABOUT BUTTON CLICKED" << std::endl;
}
