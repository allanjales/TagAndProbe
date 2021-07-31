//Change if you need
#include "src/dofits/DoFit_Jpsi_newsys.cpp"
#include "src/dofits/DoFit_Jpsi_2xGaus_for_systematic.cpp"

#include "src/create_folder.cpp"
#include "src/create_TH2D.cpp"

//Which Muon Id do you want to study?
string MuonId   = "trackerMuon";
//string MuonId   = "standaloneMuon";
//string MuonId   = "globalMuon";

//bool should_loop_muon_id  = false;

//Which quantity do you want to use?
string quantity1 = "Pt";
//double bins1[] = {0., 2.0, 3.4, 4.0, 4.4, 4.7, 5.0, 5.6, 5.8, 6.0, 6.2, 6.4, 6.6, 6.8, 7.3, 9.5, 13.0, 17.0, 40.};
double bins1[] = {0.0, 3.4, 4.0, 5.0, 6.0, 8.0, 40.};
string quantity2 = "Eta";
double bins2[] = {0.0, 0.4, 0.6, 0.95, 1.2, 1.4, 2.4};
//double bins2[] = {-2.4, -1.8, -1.4, -1.2, -1.0, -0.8, -0.5, -0.2, 0, 0.2, 0.5, 0.8, 1.0, 1.2, 1.4, 1.8, 2.4};
//string quantity = "Phi";    double bins[] = {-3.0, -1.8, -1.6, -1.2, -1.0, -0.7, -0.4, -0.2, 0, 0.2, 0.4, 0.7, 1.0, 1.2, 1.6, 1.8, 3.0};

void plot_sys_efficiency_2d()
{
	//Path where is going to save results png for every bin 
	const char* path_bins_fit_folder = "results/bins_fit/";
	create_folder(path_bins_fit_folder, true);

	// Loop for every bin and fit it
	const int nbinsy = sizeof(bins1)/sizeof(*bins1) - 1;
	const int nbinsx = sizeof(bins2)/sizeof(*bins2) - 1;

	//Creates variables to store values and error of each passed and total bin
	//Stores [yield_all, yield_pass, err_all, err_pass]
	double* yields_n_errs_Nominal  [nbinsx][nbinsy] = {0};
	double* yields_n_errs_2Gauss   [nbinsx][nbinsy] = {0};
	double* yields_n_errs_MassUp   [nbinsx][nbinsy] = {0};
	double* yields_n_errs_MassDown [nbinsx][nbinsy] = {0};
	double* yields_n_errs_BinUp    [nbinsx][nbinsy] = {0};
	double* yields_n_errs_BinDown  [nbinsx][nbinsy] = {0};


	/*
	double yields_final_pass[nbinsx][nbinsy];
	double yields_final_all [nbinsx][nbinsy];
	double errors_final_pass[nbinsx][nbinsy];
	double errors_final_all [nbinsx][nbinsy];
	*/

	double** yields_final_pass = new double*[nbinsx];
	double** yields_final_all  = new double*[nbinsx];
	double** errors_final_pass = new double*[nbinsx];
	double** errors_final_all  = new double*[nbinsx];

	for (int j = 0; j < nbinsx; j++)
	{
		yields_final_pass[j] = new double[nbinsy];
		yields_final_all [j] = new double[nbinsy];
		errors_final_pass[j] = new double[nbinsy];
		errors_final_all [j] = new double[nbinsy];

		for (int i = 0; i < nbinsy; i++)
		{
			//Creates conditions
			string conditions = string(    "ProbeMuon_" + quantity1 + ">" + to_string(bins1[i]  ));
			conditions +=       string(" && ProbeMuon_" + quantity1 + "<" + to_string(bins1[i+1]));
			conditions +=       string(" && abs(ProbeMuon_" + quantity2 + ")>" + to_string(bins2[j]  ));
			conditions +=       string(" && abs(ProbeMuon_" + quantity2 + ")<" + to_string(bins2[j+1]));

			const double default_min = _mmin;
			const double default_max = _mmax;
			string mmin_string;
			string mmax_string;

			//Nominal
			_mmin = default_min;
			_mmax = default_max;
			fit_bins = 100;
			prefix_file_name = "nominal_";
			yields_n_errs_Nominal[j][i] = doFit(conditions, MuonId, string(path_bins_fit_folder + prefix_file_name).c_str());

			//2Gauss
			_mmin = default_min;
			_mmax = default_max;
			fit_bins = 100;
			prefix_file_name = "2xgaus_";
			yields_n_errs_2Gauss[j][i] = doFit2xGaus(conditions, MuonId, string(path_bins_fit_folder + prefix_file_name).c_str());

			//MassUp
			_mmin = default_min - 0.05;
			_mmax = default_max + 0.05;
			fit_bins = 100;
			mmin_string = to_string(_mmin);
			mmax_string = to_string(_mmax);
			replace(mmin_string.begin(), mmin_string.end(), '.', 'p');
			replace(mmax_string.begin(), mmax_string.end(), '.', 'p');
			prefix_file_name  = string("mass_") + mmin_string.substr(0, mmin_string.length()-4) + string("_");
			prefix_file_name +=                   mmax_string.substr(0, mmax_string.length()-4) + string("_");
			yields_n_errs_MassUp[j][i] = doFit(conditions, MuonId, string(path_bins_fit_folder + prefix_file_name).c_str());

			//MassDown
			_mmin = default_min + 0.05;
			_mmax = default_max - 0.05;
			fit_bins = 100;
			mmin_string = to_string(_mmin);
			mmax_string = to_string(_mmax);
			replace(mmin_string.begin(), mmin_string.end(), '.', 'p');
			replace(mmax_string.begin(), mmax_string.end(), '.', 'p');
			prefix_file_name  = string("mass_") + mmin_string.substr(0, mmin_string.length()-4) + string("_");
			prefix_file_name +=                   mmax_string.substr(0, mmax_string.length()-4) + string("_");
			yields_n_errs_MassDown[j][i] = doFit(conditions, MuonId, string(path_bins_fit_folder + prefix_file_name).c_str());

			//BinUp
			_mmin = default_min;
			_mmax = default_max;
			fit_bins = 105;
			prefix_file_name = "binfit105_";
			yields_n_errs_BinUp[j][i] = doFit(conditions, MuonId, string(path_bins_fit_folder + prefix_file_name).c_str());

			//BinDown
			_mmin = default_min;
			_mmax = default_max;
			fit_bins = 95;
			prefix_file_name = "binfit95_";
			yields_n_errs_BinDown[j][i] = doFit(conditions, MuonId, string(path_bins_fit_folder + prefix_file_name).c_str());

			//Calculates the result
			yields_final_pass[j][i] = yields_n_errs_Nominal[j][i][0];
			yields_final_all [j][i] = yields_n_errs_Nominal[j][i][1];
			errors_final_pass[j][i] = sqrt(pow(yields_n_errs_Nominal[j][i][2],2) + pow(yields_n_errs_2Gauss[j][i][2],2) + pow(yields_n_errs_MassUp[j][i][2],2) + pow(yields_n_errs_MassUp[j][i][2],2) + pow(yields_n_errs_BinUp[j][i][2],2) + pow(yields_n_errs_BinDown[j][i][2],2));
			errors_final_all [j][i] = sqrt(pow(yields_n_errs_Nominal[j][i][3],2) + pow(yields_n_errs_2Gauss[j][i][3],2) + pow(yields_n_errs_MassUp[j][i][3],2) + pow(yields_n_errs_MassUp[j][i][3],2) + pow(yields_n_errs_BinUp[j][i][3],2) + pow(yields_n_errs_BinDown[j][i][3],2));

		}
	}

	//Path where is going to save efficiency
	string directoryToSave = string("results/efficiencies/") + output_folder_name + string("/");
	create_folder(directoryToSave.c_str());

	//Create file
	string file_path = directoryToSave + quantity1 + "_" + quantity2 + "_" + MuonId + ".root";
	TFile* generatedFile = new TFile(file_path.c_str(),"recreate");
	generatedFile->mkdir("histograms/");
	generatedFile->   cd("histograms/");

	create_TH2D("pass_systematic", "Pass Systematic", quantity1, quantity2, nbinsy, nbinsx, bins1, bins2, yields_final_pass, errors_final_pass);
	create_TH2D("all_systematic",  "All Systematic",  quantity1, quantity2, nbinsy, nbinsx, bins1, bins2, yields_final_all,  errors_final_all);

	generatedFile->Write();

	cout << "\n------------------------\n";
	cout << "Output: " << file_path;
	cout << "\n------------------------\n";
}