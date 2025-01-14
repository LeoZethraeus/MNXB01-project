
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <utility>
#include "TH1I.h"
#include "TLegend.h"
#include "TF1.h"
#include "TCanvas.h"
using namespace std;
/* This script takes an input file, and creates an output with the year and the hottest and coldest date.
   Design for input format of Uppsala.
   Usage:
   ./dateOfPeakTemp [mode]
   For example:
   ./dateOfPeakTemp 1
   Mode:
   1: Daily average temperature according to observations. 
   2: Daily average temperatures corrected for the urban effect.
*/

void peak_temp_Upp_old(ifstream &inFile, int start_year, int mode, int location_specified)
{
	int year = 0;
	int month = 0;
	int day = 0;
	double temp1 = 0;
	double temp2 = 0;
	double temp = 0;
	int location = 0;
	Date hottest = Date();
	Date coldest = Date();

	vector<int> num_hot(366);
	vector<int> num_cold(366);

	TH1I *hist_cold = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, 1, 367);
	TH1I *hist_hot = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, 1, 367);

	hist_cold->SetFillColor(kBlue);
	hist_cold->SetLineColor(kBlue);
	hist_hot->SetFillColor(kRed);
	hist_hot->SetLineColor(kRed);

	while (!inFile.eof())
	{
		inFile >> year >> month >> day >> temp1 >> temp2 >> location;
		//cout << year<< month <<day << temp1 << temp2 << location<< endl;
		if (location == location_specified || location_specified == 0)
		{
			if (year > start_year)
			{
				if (hottest.day_of_year() < 100)
				{
					cout << "Hottest temperature for " << start_year << " year is " << hottest.get_temp() << ", which is at " << hottest.to_string() << ", day of year " << hottest.day_of_year() << "\n";
				}
				//cout << "Coldest temperature for " << start_year << " year is " << coldest.get_temp() << ", which is at " << coldest.to_string() << ", day of year " << coldest.day_of_year() << "\n";
				//cout << "Hottest temperature for " << start_year << " year is " << hottest.get_temp() << ", which is at " << hottest.to_string() << ", day of year " << hottest.day_of_year() << "\n";
				//num_cold.at(coldest.day_of_year()-1)++;
				hist_cold->Fill(coldest.day_of_year());
				hist_hot->Fill(hottest.day_of_year());
				//num_hot.at(hottest.day_of_year()-1)++;
				start_year = year;
				hottest = Date(year, month, day);
				coldest = Date(year, month, day);
			}
			if (mode == 1)
				temp = temp1;
			else
				temp = temp2;
			if (temp > hottest.get_temp())
			{
				hottest = Date(year, month, day);
				hottest.set_temp(temp);
			}
			if (temp < coldest.get_temp())
			{
				coldest = Date(year, month, day);
				coldest.set_temp(temp);
			}
		}
	}
	//cout << "Coldest temperature for " << start_year << " year is " << coldest.get_temp() << ", which is at " << coldest.to_string() << ", day of year " << coldest.day_of_year() << "\n";
	//cout << "Hottest temperature for " << start_year << " year is " << hottest.get_temp() << ", which is at " << hottest.to_string() << ", day of year " << hottest.day_of_year() << "\n";
	hist_cold->Fill(coldest.day_of_year());
	hist_hot->Fill(hottest.day_of_year());
	//num_cold.at(coldest.day_of_year()-1)++;
	//num_hot.at(hottest.day_of_year()-1)++;
	TF1 *func_1 = new TF1("Gaussian", Gaussian, 1, 366, 3);
	TF1 *func_2 = new TF1("Gaussian", Gaussian, 1, 366, 3);
	TF1 *func_3 = new TF1("Gaussian", Gaussian, 1, 366, 3);
	func_1->SetParameters(1, 100, 50); //Starting values for fitting
	func_1->SetLineColor(kBlack);
	func_1->SetLineStyle(2);
	func_2->SetParameters(100, 300, 50); //Starting values for fitting
	func_2->SetLineColor(kBlack);
	func_3->SetParameters(300, 366, 50); //Starting values for fitting
	func_3->SetLineColor(kBlack);
	func_3->SetLineStyle(2);
	hist_cold->Fit(func_1, "QR");
	hist_hot->Fit(func_2, "QR");
	hist_cold->Fit(func_3, "QR");
	cout << "The mean for Warmest is " << func_2->GetParameter(1) << endl;
	cout << "Its uncertainty for Warmest is " << func_2->GetParError(1) << endl;
	TLegend *leg = new TLegend(0.65, 0.75, 0.92, 0.92, "", "NDC");
	leg->SetFillStyle(0);						  //Hollow fill (transparent)
	leg->SetBorderSize(0);						  //Get rid of the border
	leg->AddEntry(hist_cold, "Coldest day", "F"); //Use object title, draw fill
	leg->AddEntry(hist_hot, "Warmest day", "F");  //Use custom title
	TCanvas *can = new TCanvas();
	hist_cold->Draw();
	hist_hot->Draw("SAME");
	leg->Draw();
	can->SaveAs("./hotCold_Upp_prev.pdf");
	return;
}
