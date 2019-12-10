#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <iostream>
#include <QFileDialog>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

int doThings(QString);
int checkMode(istream&);
istream::pos_type objStartPos(istream&);
void fillVector(istream&, vector<int>&);
void getNPS(vector<int>&, vector<int>&);

static vector<int> notes;
static vector<int> notesPerSecond = {0};
static int maxNPS = 0;

int doThings(QString file){
    // Enter .osu filename here
    string fileName = file.toUtf8().constData();

    ifstream osuFile;

    osuFile.open(fileName);

    // Get angry if the file cannot be opened or they click X
    if(!osuFile){
        cout << "Can't open the file!" << endl;
        return -1;
    }

    int modePos = checkMode(osuFile);
    if(modePos != 3){
        return -2;
    }

    // Find the starting position of the notes
    istream::pos_type pos = objStartPos(osuFile);

    // Move to the beginning of the notes
    osuFile.seekg(pos);

    // Fill the vector with the positions of all the notes
    fillVector(osuFile, notes);

    // Get the nps for every second of the song and put info into the vector
    getNPS(notes, notesPerSecond);

/*
    cout << "NPS vector test: " << endl;
    for(auto j = notesPerSecond.begin() + 1; j != notesPerSecond.end(); j++){
        cout << *j << endl;
    }
*/

    return 0;
}


int checkMode(istream& osuFile){
    const string mode = "Mode";
    string line;
    istream::pos_type lineStart = osuFile.tellg();

    while(getline(osuFile, line)){
//		cout << "Start: " <<  lineStart << endl;

        if (line.find(mode) != string::npos){
//			cout << line << endl;
            stringstream ss(line);
            string mode_s;
            int counter = 0;
            while(getline(ss, mode_s, ':')){
                if(counter == 1){
                    return stoi(mode_s);
                }
                counter++;
            }
        }

        istream::pos_type lineEnd = osuFile.tellg();
        lineStart = lineEnd;
//		cout << "End: " << lineEnd << endl;
    }
    cout << "This is not an osu!mania file!" << endl;
    return -2;
}

/*
This function skims through the file until the beginning of the HitObjects (aka the notes)
*/
istream::pos_type objStartPos(istream& osuFile){
    const string objStart = "[HitObjects]";
    string line;
    istream::pos_type lineStart = osuFile.tellg();

    while(getline(osuFile, line)){
//		cout << "Start: " <<  lineStart << endl;

        if (line.find(objStart) != string::npos){
//			cout << line << endl;

            // Back up the position to account for newline characters (\r\n)
            char moreChars[14];
            istream::pos_type addChars = sizeof(moreChars);

            // Return the position where the notes begin
            return lineStart + addChars;
        }

        istream::pos_type lineEnd = osuFile.tellg();
        lineStart = lineEnd;
//		cout << "End: " << lineEnd << endl;
    }
    cout << "This is not a .osu file!" << endl;
    return -1;
}

/*
This function takes the timing points of all the notes and puts them into a vector
*/
void fillVector(istream& osuFile, vector<int>& notes){
    string section;
    int counter = 1;

    while(getline(osuFile, section)){
        stringstream ss(section);
        while(getline(ss, section, ',')){
            if(counter % 3 == 0 && counter % 2 != 0){
//				cout << section << ": " << counter << endl;
                int section_i = stoi(section);
                notes.push_back(section_i);
            }
            counter++;
        }
    }
}

/*
This function finds the notes per second for each second in the song and puts them in a vector
*/
void getNPS(vector<int>& notes, vector<int>& notesPerSecond){
    int min = notes.front();
    int max = notes.front() + 1000;
    int testCounter = 1;
    int nps = 0;

    while(min <= notes.back()){
        for(auto i = notes.begin(); i != notes.end(); i++){
            if(*i > min && *i <= max){
//				cout << *i << endl;
                nps++;
            }
        }
        min = max;
        max = min + 1000;
//      cout << "NPS for second " << testCounter << ": " << nps << endl;
        notesPerSecond.push_back(nps);
        if(nps > maxNPS){
            maxNPS = nps;
        }
        nps = 0;
        testCounter++;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir osuDir = appData;
    osuDir.cdUp();
    osuDir.cd("osu");
    osuDir.cd("Songs");
    QString songsLocation = osuDir.path();

selectFile:
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select a Beatmap"), songsLocation, tr("Beatmaps (*.osu)"));

    int error = doThings(fileName);

    if(error == 0){
        QVector<double> x(notesPerSecond.size()), y(notesPerSecond.size());
        for (int i = 1; i != notesPerSecond.size(); ++i)
        {
            x[i] = i;
            y[i] = notesPerSecond[i];
        }

        // new bar graph
        QCPBars *newBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
        QBrush *brush = new QBrush(Qt::SolidPattern);
        QPen *pen = new QPen(Qt::SolidLine);

        newBars->setWidth(1);
        brush->setColor(Qt::darkCyan);
        newBars->setBrush(*brush);
        pen->setColor(Qt::black);
        pen->setWidth(1);
        newBars->setPen(*pen);

        // create graph and assign data to it:
        ui->customPlot->addGraph();
        newBars->setName("Notes Per Second");
        newBars->setData(x, y);

        // give the axes some labels:
        ui->customPlot->xAxis->setLabel("Seconds");
        ui->customPlot->yAxis->setLabel("NPS");

        // set axes ranges, so we see all data:
        ui->customPlot->xAxis->setRange(0, notesPerSecond.size());
        ui->customPlot->yAxis->setRange(0, maxNPS + 1);
        ui->customPlot->replot();
    }
    else if(error == -1){
        exit(EXIT_FAILURE);
    }
    else{
        QMessageBox msgBox(QMessageBox::Critical, "Notes Per Second", "This program only works with osu!mania files!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.exec();
        goto selectFile;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

