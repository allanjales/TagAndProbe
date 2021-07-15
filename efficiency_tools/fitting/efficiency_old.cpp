#include "src/compare_efficiency.cpp"

//Change if you need
#include "src/dofits/DoFit_Jpsi.cpp"
//#include "src/dofits/DoFit_Jpsi_2xGaus.cpp"
double default_min = _mmin;
double default_max = _mmax;

#include "src/create_folder.cpp"
#include "src/get_efficiency.cpp"
#include "src/change_bin.cpp"
#include "src/make_hist.cpp"

//Which Muon Id do you want to study?
string MuonId   = "trackerMuon";
//string MuonId   = "standaloneMuon";
//string MuonId   = "globalMuon";

//Which quantity do you want to use?
//string quantity = "Pt";     double bins[] = {0., 2.0, 3.4, 4.0, 4.4, 4.7, 5.0, 5.6, 5.8, 6.0, 6.2, 6.4, 6.6, 6.8, 7.3, 9.5, 13.0, 17.0, 40.};
//string quantity = "Eta";    double bins[] = {-2.4, -1.8, -1.4, -1.2, -1.0, -0.8, -0.5, -0.2, 0, 0.2, 0.5, 0.8, 1.0, 1.2, 1.4, 1.8, 2.4};
//string quantity = "Phi";    double bins[] = {-3.0, -1.8, -1.6, -1.2, -1.0, -0.7, -0.4, -0.2, 0, 0.2, 0.4, 0.7, 1.0, 1.2, 1.6, 1.8, 3.0};

string quantity = "Pt";     double bins[] = {0.0, 2.0, 3.4, 4.0, 5.0, 6.0, 8.0, 10.0, 40.};
//string quantity = "Eta";    double bins[] = {0.0, 0.4, 0.6, 0.95, 1.2, 1.4, 1.6, 1.8, 2.1};

bool should_loop_muon_id  = false;
bool should_loop_settings = true;

void efficiency_calculus()
{
	//_mmin = 2.75;
	//_mmax = 3.35;
	//fit_bins = 95;
	//prefix_file_name = "mass_2p75_3p35_";
	//prefix_file_name = "2xgaus_";

	//Path where is going to save results png for every bin 
	const char* path_bins_fit_folder = "results/bins_fit/";
	create_folder(path_bins_fit_folder, true);

	// Loop for every bin and fit it
	int bin_n = sizeof(bins)/sizeof(*bins) - 1;
	double** yields_n_errs = new double*[bin_n];
	for (int i = 0; i < bin_n; i++)
	{
		//Creates conditions
		string conditions = string(    "ProbeMuon_" + quantity + ">" + to_string(bins[i]  ));
		conditions +=       string(" && ProbeMuon_" + quantity + "<" + to_string(bins[i+1]));

		//Stores [yield_all, yield_pass, err_all, err_pass]
		yields_n_errs[i] = doFit(conditions, MuonId, quantity, path_bins_fit_folder);
	}
	
	TH1F *yield_ALL  = make_hist("ALL" , yields_n_errs, 0, bin_n, bins);
	TH1F *yield_PASS = make_hist("PASS", yields_n_errs, 1, bin_n, bins);
	
	//----------------------SAVING RESULTS TO Histograms.root--------------------//
	//useful if we require to change the fit on a specific set of bins
	TFile* EfficiencyFile = TFile::Open((string(path_bins_fit_folder) + "histograms.root").c_str(),"RECREATE");
	yield_ALL->SetDirectory(gDirectory);
	yield_PASS->SetDirectory(gDirectory);
	EfficiencyFile->Write();
	//-----------------------------------------------------------------//
	
	//If all of the fits seem correct we can proceed to generate the efficiency
	get_efficiency(yield_ALL, yield_PASS, quantity, MuonId, prefix_file_name);
	 
	//In case you want to change the fit on a specific, comment the loop and "result saving" code and uncomment the following function
	//change_bin(/*bin number you want to redo*/, /*condition (you can copy the title from the generated fit .png)*/, MuonId, quantity, init_conditions);
	//bins start on 1
	
	//Once we've calculated the efficiency for both data sets, we can generate
	//a plot that combines both results
	//compare_efficiency(quantity, "Efficiency Result/" + quantity + "/Efficiency_MC.root", "Efficiency Result/" + quantity + "/Efficiency_Run2011.root");

	cout << "\n[Settings]\n";
	cout << output_folder_name << " "<< MuonId << " " << quantity << "\n";
	cout << "Fitting:     " << fit_functions << "\n";
	cout << "Fit between: " << _mmin << " and " << _mmax << " GeV\n";
	cout << "Bins:        " << fit_bins << "\n";
}

void loop_settings()
{
	string min_string = "";
	string max_string = "";
	for (int i = 0; i <= 4; i++)
	{
		switch(i)
		{
			case 0:
				prefix_file_name = "nominal_";
				break;
			case 1:
				_mmin = default_min - 0.05;
				_mmax = default_max + 0.05;
				min_string = to_string(_mmin);
				max_string = to_string(_mmax);
				replace(min_string.begin(), min_string.end(), '.', 'p');
				replace(max_string.begin(), max_string.end(), '.', 'p');
				prefix_file_name  = string("mass_") + min_string.substr(0, min_string.length()-4) + string("_");
				prefix_file_name +=                   max_string.substr(0, max_string.length()-4) + string("_");
				break;
			case 2:
				_mmin = default_min + 0.05;
				_mmax = default_max - 0.05;	
				min_string = to_string(_mmin);
				max_string = to_string(_mmax);
				replace(min_string.begin(), min_string.end(), '.', 'p');
				replace(max_string.begin(), max_string.end(), '.', 'p');
				prefix_file_name  = string("mass_") + min_string.substr(0, min_string.length()-4) + string("_");
				prefix_file_name +=                   max_string.substr(0, max_string.length()-4) + string("_");
				break;
			case 3:
				fit_bins = 95;
				prefix_file_name = "binfit95_";
				break;
			case 4:
				fit_bins = 105;
				prefix_file_name = "binfit105_";
				break;
		}
		efficiency_calculus();
	}
}

void loop_muon_id()
{
	for (int i = 0; i <= 2; i++)
	{
		switch(i)
		{
			case 0:
				MuonId   = "trackerMuon";
				break;
			case 1:
				MuonId   = "standaloneMuon";
				break;
			case 2:
				MuonId   = "globalMuon";
				break;
		}		

		if (should_loop_settings)
			loop_settings();
		else
			efficiency_calculus();
	}
}

void efficiency()
{
	if (should_loop_muon_id)
		loop_muon_id();
	else if (should_loop_settings)
		loop_settings();
	else
		efficiency_calculus();
}