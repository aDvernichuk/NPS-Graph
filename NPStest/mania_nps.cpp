/*
TODO: 	- learn how to make graphs
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

istream::pos_type objStartPos(istream&);
void fillVector(istream&, vector<int>&);
void getNPS(vector<int>&, vector<int>&);

int main(){
	// Enter .osu filename here
	string fileName = "C:\\Users\\yes\\AppData\\Local\\osu\\Songs\\292994 ueotan - Mario Paint (Time Regression Mix for BMS)\\ueotan - Mario Paint (Time Regression Mix for BMS) (Fullerene-) [D-ANOTHER].osu";

	vector<int> notes;
	vector<int> notesPerSecond = {0};

	ifstream osuFile;

	osuFile.open(fileName);

	// Get angry if the file cannot be opened
	if(!osuFile){
		cout << "Can't open the file!" << endl;
		return 1;
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
			char newlineSize[4];
			istream::pos_type removeNL = sizeof(newlineSize);

			// Return the position where the notes begin
			return lineStart - removeNL;
        }

		istream::pos_type lineEnd = osuFile.tellg();
		lineStart = lineEnd;
//		cout << "End: " << lineEnd << endl;
    }
    cout << "Something is wrong with your file." << endl;
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
		cout << "NPS for second " << testCounter << ": " << nps << endl;
		notesPerSecond.push_back(nps);
		nps = 0;
		testCounter++;
	}
}
