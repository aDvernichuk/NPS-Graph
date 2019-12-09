#include "mainwindow.h"

#include <QApplication>

using namespace std;

istream::pos_type objStartPos(istream&);
void fillVector(istream&, vector<int>&);
void getNPS(vector<int>&, vector<int>&);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
