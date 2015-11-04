#include <qapplication.h>
#include "ApplicationManager.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ApplicationManager app(&a);
	app.start();
}
