/*

	For this file, it should be sufficient to modify the function "processFile()" only.

*/


#include <windows.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <strstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#include "li.h"
#include "lmath.h"
#include "pointSet.h"
#include "stopWatch.h"

static StopWatch globalSW;
static string groupName = "THE INVINCIBLE FOUR";

void processFile(char* ifilename, char* ofilename){

	string line_noStr;

	string line;   // each line of the file
	string command;// the command of each line
	string numberStr; // for single LongInt operation
	string outputAns = "Answer of your computation"; // the answer you computed

	ifstream inputFile(ifilename,ios::in);
	ofstream outputFile(ofilename,ios::out);

	LongInt currentValue;
	PointSet myPointSet;

	if(inputFile.fail()){
		cerr << "Error: Cannot read input file \"" << ifilename << "\"";
		exit(1);
	}

	while(inputFile.good()){

		getline(inputFile,line);
		if(line.empty()) {
			command = "";
			continue; 
		}// in case the line has nothing in it

		stringstream linestream(line);

		linestream >> line_noStr;
		linestream >> command;         // get the command

		if(!command.compare("LX")){
			linestream >> numberStr;

			currentValue = LongInt::LongInt(numberStr.c_str());
			//outputAns = currentValue.printOut();

			globalSW.pause();
			outputFile << line_noStr  << numberStr << endl;
			globalSW.resume();
		}
		else if(!command.compare("AD")){
			linestream >> numberStr;

			LongInt newNumber = LongInt::LongInt(numberStr.c_str());			
			currentValue = currentValue+(newNumber); // currentValue + newNumber
			outputAns = currentValue.printOut();

			globalSW.pause();
			outputFile << line_noStr  << outputAns << endl;
			globalSW.resume();
		}
		else if(!command.compare("SB")){
			linestream >> numberStr;

			LongInt newNumber = LongInt::LongInt(numberStr.c_str());			
			currentValue = currentValue-(newNumber); // currentValue - newNumber
			outputAns = currentValue.printOut();

			globalSW.pause();
			outputFile << line_noStr  << outputAns << endl;
			globalSW.resume();
		}
		else if(!command.compare("MU")){
			linestream >> numberStr;

			LongInt newNumber = LongInt::LongInt(numberStr.c_str());
			currentValue = newNumber * currentValue;
			outputAns = currentValue.printOut();

			globalSW.pause();
			outputFile << line_noStr  << " " << outputAns << endl;
			globalSW.resume();
		}
		else if(!command.compare("NG")){
			linestream >> numberStr;
			
			currentValue = -currentValue;
			outputAns = currentValue.printOut();

			globalSW.pause();
			outputFile << line_noStr  << " " << outputAns << endl;
			globalSW.resume();
		}
		else if(!command.compare("SD")){
			// you can assume that the three rows of matrix are in three separate lines in the file
			//linestream >> numberStr;
			//getline(inputFile,line);
			//getline(inputFile,line);		
			//getline(inputFile,line);

			std::vector<LongInt> longInts;
			for (int i = 0; i < 3; i++) {
				getline(inputFile,line);
				linestream = stringstream(line);
				while (linestream >> numberStr){
					LongInt temp = LongInt(numberStr);
					longInts.push_back(temp);
				}
			}
			int ans = signDet(longInts[0], longInts[1], longInts[2], 
				longInts[3], longInts[4], longInts[5], 
				longInts[6], longInts[7], longInts[8]);

			ostringstream convert; 
			convert << ans;
			outputAns = convert.str();

			globalSW.pause();
			outputFile << line_noStr  << " " << outputAns << endl;
			globalSW.resume();
		}
		else if(!command.compare("AP")){
			linestream >> numberStr;
			LongInt p1 = LongInt::LongInt(numberStr.c_str());

			linestream >> numberStr;
			LongInt p2 = LongInt::LongInt(numberStr.c_str());

			int output = myPointSet.addPoint(p1, p2);
			ostringstream convert;
			convert << output;
			outputAns = "#POINT = " + convert.str();

			globalSW.pause();
			outputFile << line_noStr  << " " << outputAns << endl;
			globalSW.resume();
		}
		else if(!command.compare("IT")){
			linestream >> numberStr;
			int pIdx = atoi(numberStr.c_str());
			linestream >> numberStr;
			int p1Idx = atoi(numberStr.c_str());
			linestream >> numberStr;
			int p2Idx = atoi(numberStr.c_str());
			linestream >> numberStr;
			int p3Idx = atoi(numberStr.c_str());

			int output = myPointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx);

			ostringstream convert;
			convert << output;
			outputAns = convert.str();

			globalSW.pause();
			outputFile << line_noStr  << " " << outputAns << endl;
			cout<<line_noStr<<endl;
			globalSW.resume();
		}
		else if(!command.compare("IC")){
			linestream >> numberStr;
			int pIdx = atoi(numberStr.c_str());
			linestream >> numberStr;
			int p1Idx = atoi(numberStr.c_str());
			linestream >> numberStr;
			int p2Idx = atoi(numberStr.c_str());
			linestream >> numberStr;
			int p3Idx = atoi(numberStr.c_str());

			int output = myPointSet.inCircle(p1Idx, p2Idx, p3Idx, pIdx);

			ostringstream convert;
			convert << output;
			outputAns = convert.str();

			globalSW.pause();
			outputFile << line_noStr  << " " << outputAns << endl;
			globalSW.resume();
		}
		else{
			cerr << "Exception: Wrong input command" << endl;
		}
	}

}

void runningExperiment()
{
	globalSW.reset();
	globalSW.resume();
	processFile("input20.txt", "output.txt");
	globalSW.pause();
}

int main()
{
    SYSTEMTIME st;
	const int no_exp = 5;
	ULONGLONG t[no_exp];
	ULONGLONG total_t = 0;
    
    GetLocalTime(&st);
	int start = (((st.wHour*60+st.wMinute)*60)+st.wSecond)*1000+st.wMilliseconds;

	cerr << groupName <<endl;
	cerr << "Start Time:  " << st.wHour << ":" << st.wMinute << ":" << st.wSecond + (st.wMilliseconds/1000.0) << endl;

	for(int i=0;i<1;i++)
	{
		runningExperiment();
		t[i] = globalSW.ms();
		total_t += t[i];
	}

    GetLocalTime(&st);
	int end = (((st.wHour*60+st.wMinute)*60)+st.wSecond)*1000+st.wMilliseconds;
	cerr << "Ending Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond + (st.wMilliseconds/1000.0) << endl;
	cerr << "Overall Length in millisecond(including file output): " << end-start << endl;
	cerr << "Excluding file output time:"<< endl;
		cerr << "         Total Stopwatch Length(ms): " << total_t << endl;

	for(int i=0;i<no_exp;i++)
		cerr << "         Stopwatch" << (i+1) << "Length(ms): " << t[i] << endl;
	cerr << "         Average Stopwatch Length(ms): " << total_t/no_exp << endl;
   
	system("pause");
}