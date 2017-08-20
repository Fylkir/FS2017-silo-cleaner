#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <list>
#include <vector>
#include <algorithm>


using namespace std;
using namespace System;

struct Silo
{
	int line;
	int id;
	double FillLevel;
	bool operator<(Silo& s)
	{
		return id < s.id;
	}
};

string MarshalString(System::String ^ s);
string getSaveFilePath();
double findSiloFillLevel(string line);
int findSiloID(string line);
void findSilos(string path, Silo(&silos)[6]);
void cleanSilos(string path, Silo(&silos)[6], vector<int> ids);




int main()
{

	const string path = getSaveFilePath();
	Silo silos[6];

	findSilos(path, silos);
	sort(silos, silos + 6);


	cout << "6 SILOS FOUND" << endl;
	for (int i = 0; i < 6; i++)
	{
		if (silos[i].id < 6) cout << "Silos no. "<< silos[i].id <<"\t\t\t Fill level: " << silos[i].FillLevel <<  endl;
		else cout << "Silos no " << silos[i].id << " (near the barn)" << "\t Fill level: " << silos[i].FillLevel << endl;
	}
	
	cout << endl << endl << "Please insert numbers of silos which you want to clear:" << endl;
	
	
	
	
	int input=10;
	vector<int> silosToClean;

	while (input != 0)
	{
		cin >> input;
		if (input > 0 && input < 7)
		{
			silosToClean.push_back(input);
			cout << "The following silos will be cleared: ";
			for (int x : silosToClean) cout << x << " ";
			cout << endl << "Please insert a number of the next silo, or insert 0 to finish:" << endl;
		}
		else
		{
			if (input !=0) cout << "Invalid number." << endl;
			else cout << "Finish has been chosen. Silos are being cleared" << endl;
		}

	}
	
	cleanSilos(path, silos, silosToClean);

	cout << "Silos: ";
	for (int x : silosToClean) cout << x << " ";
	cout << "Have been cleared." << endl;





	getchar();
	getchar();
	return 0;
}

string MarshalString(System::String ^ s)
{
	using namespace System::Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	string os = chars;
	Marshal::FreeHGlobal(System::IntPtr((void*)chars));
	return os;
}

string getSaveFilePath()
{


	String^ path_S = Environment::GetFolderPath(Environment::SpecialFolder::MyDocuments);

	string path = MarshalString(path_S);
	path = path + "\\My Games\\FarmingSimulator2017\\savegame1\\vehicles.xml";

	return path;

}

double findSiloFillLevel(string line)
{
	smatch res;
	regex siloFillLevel("fillLevel=\"[0-9]+\\.*[0-9]*");
	regex number("[0-9]+\\.*[0-9]*");
	regex_search(line, res, siloFillLevel);
	string fillLevel = res.str(0);
	regex_search(fillLevel, res, number);
	string::size_type sz;
	return stod(res.str(0), &sz);
}

int findSiloID(string line)
{
	string::size_type sz;
	smatch res;
	regex bunkerName("BunkerSilo_silo[0-9]+");
	regex id("[0-9]+");

	regex_search(line, res, bunkerName);

	string name = res.str(0);
	if (!regex_search(name, res, id))
	{
		return 6;
	}
	else
	{
		return stoi(res.str(0), &sz);
	}
}

void findSilos(string path, Silo(&silos)[6])
{
	fstream f;
	f.open(path, fstream::in | fstream::out);

	string line;
	smatch res;

	regex isSiloHere("^.*<onCreateLoadedObject saveId=\"BunkerSilo.*");


	int i = 0;
	int siloCount = 0;

	while (getline(f, line) && siloCount <= 6)
	{
		i++;
		if (regex_search(line, res, isSiloHere))
		{
			silos[siloCount].id = findSiloID(line);
			silos[siloCount].line = i;
			silos[siloCount].FillLevel = findSiloFillLevel(line);
			siloCount++;
		}
	}

	f.close();

}

void cleanSilos(string path, Silo(&silos)[6], vector<int> ids)
{
	fstream f;
	f.open(path, fstream::in);

	string line;
	list<string> fileContents;
	list<string>::iterator it;
	vector<int>::iterator itv;

	while (getline(f, line))
	{
		fileContents.push_back(line);
	}

	f.close();

	ofstream of;

	of.open(path, ofstream::trunc | ofstream::out);



	sort(silos, silos + 6, [](Silo s1, Silo s2) {return s1.line < s2.line; });

	int i = 0;
	int curSil = 0;
	for (it = fileContents.begin(); it != fileContents.end(); it++)
	{
		i++;
		if (silos[curSil].line == i)
		{
			int id = silos[curSil].id;
			itv = find(ids.begin(), ids.end(), id);
			if (itv != ids.end())
			{
				if (id != 6) of << "    <onCreateLoadedObject saveId=\"BunkerSilo_silo00" + to_string(id) + "\" state=\"0\" fillLevel=\"0\" compactedFillLevel=\"0\" fermentingTime=\"0\" fermentingPercent=\"0\" openedAtFront=\"false\" openedAtBack=\"false\" />" << endl;

				else of << "    <onCreateLoadedObject saveId=\"BunkerSilo_cowSilo\" state=\"0\" fillLevel=\"0\" compactedFillLevel=\"0\" fermentingTime=\"0\" fermentingPercent=\"0\" openedAtFront=\"false\" openedAtBack=\"false\" />" << endl;
			}
			else of << *it << endl;

			curSil++;
		}
		else of << *it << endl;

	}

	of.close();


}
