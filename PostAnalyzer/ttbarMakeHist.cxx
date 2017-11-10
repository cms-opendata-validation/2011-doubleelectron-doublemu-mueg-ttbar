// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// This code processes ROOT ntuples for ttbar analysis (see 
// Analyzer/src/Analyzer.cc) and produces histograms, which are 
// further used to make final plots (see ttbarMakePlots.cxx).
// Run: ./ttbarMakeHist
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//
// additional files from this analysis (look there for description) 
#include "eventReco.h"
#include "settings.h"
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>> Main function >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char** argv)
{
  //
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>> Settings >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // set directories to data and MC ntuples
  TString dataDir = gDataDir;
  TString mcDir = gMcDir;
  //
  // flags what to run
  bool flagData    = 1; // if 1, data will be processed
  bool flagMCsig   = 1; // if 1, signal MC (dileptonic decay channel) will be processed
  bool flagMCstop  = 1; // if 1, MC single top (background) will be processed
  bool flagMCwjets = 1; // if 1, MC W+jets (background) will be processed
  bool flagMCdy    = 1; // if 1, MC Drell-Yan (background) will be processed
  //
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // common purpose variables
  std::vector<TString> nameInFile; // container to store input file names
  
  // histograms
  TH1::SetDefaultSumw2(); // keep histogram weights by default
  // ZVarHisto is a simple class which incorporates a histogram and a variable name. 
  // This class is used to store needed input settings (variable names, binning) 
  // for control plots and cross sections (as in TOP-11-013)
  std::vector<ZVarHisto> vecVH, vecVHGen; // vecVH for reconstruction level, vecVHGen for generator level
  // histograms and variables for control plots
  vecVHGen.push_back(ZVarHisto("ptt", new TH1D("h_ptt", "pT top", 21, 0.0, 420.0))); // pT(top) (pT = transeverse momentum)
  vecVHGen.push_back(ZVarHisto("ptat", new TH1D("h_ptat", "pT atop", 21, 0.0, 420.0))); // pT(antitop)
  vecVHGen.push_back(ZVarHisto("pttat", new TH1D("h_pttat", "pT tatop", 21, 0.0, 420.0))); // pT(top)+pT(antitop)
  vecVHGen.push_back(ZVarHisto("pttt", new TH1D("h_pttt", "pT ttbar", 30, 0.0, 300.0))); // pT(ttbar), ttbar = top + antitop
  vecVHGen.push_back(ZVarHisto("yt", new TH1D("h_yt", "y top", 26, -2.6, 2.6))); // y(top) (y = rapidity)
  vecVHGen.push_back(ZVarHisto("yat", new TH1D("h_yat", "y atop", 26, -2.6, 2.6))); // y(antitop)
  vecVHGen.push_back(ZVarHisto("ytat", new TH1D("h_ytat", "y tatop", 26, -2.6, 2.6))); // y(top)+y(antitop)
  vecVHGen.push_back(ZVarHisto("ytt", new TH1D("h_ytt", "y ttbar", 26, -2.6, 2.6))); // y(ttbar)
  // histograms and variables for cross sections
  {
    double bins[] = {0.,80.,130.,200.,300.,400.};
    vecVHGen.push_back(ZVarHisto("ptt", new TH1D("h_ptt_cs", "pT top", 5, bins)));
    vecVHGen.push_back(ZVarHisto("ptat", new TH1D("h_ptat_cs", "pT atop", 5, bins)));
    vecVHGen.push_back(ZVarHisto("pttat", new TH1D("h_pttat_cs", "pT tatop", 5, bins)));
  }
  {
    double bins[] = {-2.5,-1.3,-0.8,-0.4,0.0,0.4,0.8,1.3,2.5};
    vecVHGen.push_back(ZVarHisto("yt", new TH1D("h_yt_cs", "y top", 8, bins)));
    vecVHGen.push_back(ZVarHisto("yat", new TH1D("h_yat_cs", "y atop", 8, bins)));
    vecVHGen.push_back(ZVarHisto("ytat", new TH1D("h_ytat_cs", "y tatop", 8, bins)));
  }
  {
    double bins[] = {0.,20.,60.,120.,300.};
    vecVHGen.push_back(ZVarHisto("pttt", new TH1D("h_pttt_cs", "pT ttbar", 4, bins)));
  }
  {
    double bins[] = {-2.5,-1.5,-0.7,0.0,0.7,1.5,2.5};
    vecVHGen.push_back(ZVarHisto("ytt", new TH1D("h_ytt_cs", "y ttbar", 6, bins)));
  }
  {
    double bins[] = {0.,345.,400.,470.,550.,650.,800.,1100.,1600.};
    vecVHGen.push_back(ZVarHisto("mtt", new TH1D("h_mtt_cs", "M ttbar", 8, bins)));
  }

  // for reconstruction level the same binning is needed
  vecVH = vecVHGen;
  // add lepton pT histogram at reconstruction level
  // (here you can add more reconstruction level histograms)
  vecVH.push_back(ZVarHisto("ptl", new TH1D("h_ptl", "pT leptons", 23, 30.0, 260.0)));
  
  // loop over decay channels (ch = 1 ee, ch = 2 mumu, ch = 3 emu)
  for(int ch = 1; ch <= 3; ch++)
  {
    //if(ch != 3) continue; // if you need only emu (for test purpose e.g.)
    // 
    // below similar pieces of code come for data and several MC samples, 
    // detailed description is given for the first piece, while later on 
    // only new features are described
    //
    // *****************************************
    // **************** DATA *******************
    // *****************************************
    if(flagData)
    {
      // ZEventRecoInput is a class for event reconstruction, see its description in eventReco.h
      ZEventRecoInput in;
      //in.MaxNEvents = 100; // if you need to limit the number of processed events
      in.Name = "data"; // name pattern for output histograms
      in.Type = 1; // type = 1 for data, 2 for MC signal, 3 for MC 'ttbar other', 4 for the rest of MC background samples
      in.Channel = ch; // decay channel
      in.VecVarHisto = vecVH; // need to copy it, because further will be changed
      // input ROOT ntuples
      if(ch == 1) // ee
        in.AddToChain(dataDir + "/DoubleElectron/*.root");
      else if(ch == 2) // mumu
        in.AddToChain(dataDir + "/DoubleMu/*.root");
      else if(ch == 3) // emu
        in.AddToChain(dataDir + "/MuEG/*.root");
      // main part: event reconstruction call
      eventreco(in);
    }
    
    // *****************************************
    // ************** MC signal ****************
    // *****************************************
    //
    // MC event weights need to be changed to most precise theoretical predictions, 
    // the formula is:
    // weight = lumi / (nevents / sigma_MC) * (sigma_theory / sigma_MC) = lumi * nevents / sigma_theory
    //
    // Number of events can be obtained from webpage (see http://opendata.cern.ch/collection/CMS-Simulated-Datasets), 
    // but it should be checked that all events have been processed at the Analyzer step (see end of log files)
    //
    // number of events: 54990752
    // MC cross section -> theory: 95.43 -> 165.6
    // weight: 2500.0 / (54990752. / 95.43) * (165.6 / 95.43) = 0.007529
    if(flagMCsig)
    {
      // MC signal reco level
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Weight = 0.007529; // weight (see above)
      in.Name = "mcSigReco";
      in.Type = 2;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      in.AddToChain(mcDir + "/TTJets_TuneZ2_7TeV-madgraph-tauola/00001/*.root");
      in.AddToChain(mcDir + "/TTJets_TuneZ2_7TeV-madgraph-tauola/010000/*.root");
      in.AddToChain(mcDir + "/TTJets_TuneZ2_7TeV-madgraph-tauola/010003/*.root");
      in.AddToChain(mcDir + "/TTJets_TuneZ2_7TeV-madgraph-tauola/010002/*.root");
      in.AddToChain(mcDir + "/TTJets_TuneZ2_7TeV-madgraph-tauola/010001/*.root");
      in.AddToChain(mcDir + "/TTJets_TuneZ2_7TeV-madgraph-tauola/00000/*.root");
      eventreco(in);
      // MC ttbar other (background): re-use existing ZEventRecoInput, just change type
      in.Name = "mcSigOtherReco";
      in.Type = 3;
      eventreco(in);
      // MC ttbar signal, generator level: again re-use existing ZEventRecoInput, change type and set proper flag (see below)
      in.Name = "mcSigGen";
      in.Type = 2;
      in.VecVarHisto = vecVHGen;
      in.Gen = true; // flag to notify that generator level should be processed
      eventreco(in);
    }
    // *****************************************
    // ************ MC single top **************
    // *****************************************
    // number of events: 744859 + 801626
    // MC cross section -> theory: 7.475 -> 7.87
    // weight: 2500.0 / ((744859. + 801626.) / (7.475 * 2.)) * (7.87 / 7.475) = 0.02544
    if(flagMCstop)
    {
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Name = "mcSingleTopReco";
      in.Type = 4;
      in.Weight = 0.02544;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      in.AddToChain(mcDir + "/Tbar_TuneZ2_tW-channel-DR_7TeV-powheg-tauola/*.root");
      in.AddToChain(mcDir + "/T_TuneZ2_tW-channel-DR_7TeV-powheg-tauola/*.root");
      eventreco(in);
    }
    // *****************************************
    // ************** MC W+jets ****************
    // *****************************************
    // number of events: 78347691
    // MC cross section -> theory: 25430 -> 31314
    // weight: 2500.0 / (78347691. / (25430. * 0.32)) * (31314. / 25430.) = 0.3197
    if(flagMCwjets)
    {
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Name = "mcWjetsReco";
      in.Weight = 0.3197;
      in.Type = 4;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      in.AddToChain(mcDir + "/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/*.root");
      eventreco(in);
    }
    // *****************************************
    // **************** MC DY ******************
    // *****************************************
    // here separate samples exist for low and high masses, 
    // therefore separate weights calculated below
    if(flagMCdy)
    {
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Type = 4;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      // low mass
      // number of events: 39909640
      // MC cross section -> theory: 9487 -> 11908
      // weight: 2500.0 / (39909640. / (9487. * 0.1)) * (11908. / 9487.) = 0.07459
      in.Name = "mcDYlmReco";
      in.Weight = 0.07459;
      in.AddToChain(mcDir + "/DYJetsToLL_M-10To50_TuneZ2_7TeV-pythia6/*.root");
      eventreco(in);
      // high mass
      // Events: 36408225
      // MC cross section -> theory: 2513 -> 3048
      // weight: 2500.0 / (36408225. / (2513. * 0.1)) * (3048. / 2513.) = 0.02093
      in.Name = "mcDYhmReco";
      in.Weight = 0.2093;
      in.ClearChain();
      in.AddToChain(mcDir + "/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/*.root");
      eventreco(in);
    }
  }

  return 0;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
