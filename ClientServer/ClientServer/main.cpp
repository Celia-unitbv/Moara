#include "StartWindow.h"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{

	QApplication a(argc, argv);

	StartWindow Sw;
	Sw.show();

	return a.exec();
}
