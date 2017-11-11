// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// This code processes ROOT histograms for ttbar analysis,
// (produced by ttbarMakeHist.cxx), and makes final plots and numbers
// (more precisely, control plots to be compared to TOP-11-013 Fig. 4, 
// normalised cross sections to be compared to TOP-11-013 Fig. 10 
// and the total cross section to be compared to TOP-13-004).
// Run: ./ttbarMakePlots
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// additional files from this analysis 
#include "plots.h"
#include "settings.h"
// C++ library or ROOT header files
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGaxis.h>
#include <TFile.h>
#include <TLegend.h>
#include <TGraphAsymmErrors.h>

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>> Prepare plot style >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//
// Modify as you want, if needed consult 
// https://root.cern.ch/doc/master/classTStyle.html 
//
void Style()
{
	gStyle->SetOptStat(000000000);
	gStyle->SetTitle(0);
	gStyle->SetFrameFillColor(0);
	gStyle->SetPadColor(0);
	gStyle->SetCanvasColor(0);
	gStyle->SetStatColor(0);
	gStyle->SetCanvasBorderMode(0);
	gStyle->SetCanvasBorderSize(0);
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameBorderSize(0);
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	//gStyle->SetPadGridX(1);
	//gStyle->SetPadGridY(1);
	gStyle->SetLegendBorderSize(0);
	gStyle->SetEndErrorSize(5);
  TGaxis::SetMaxDigits(3);
  gStyle->SetErrorX(0.0);
  gStyle->SetPadLeftMargin(0.18);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadTopMargin(0.06);
  gStyle->SetPadRightMargin(0.08);
  //gStyle->SetNdivisions(206, "xyz");
}

// Additional tuning for plotted histograms (font sizes etc.)
void SetCPHRange(TH2* h)
{
  h->GetXaxis()->SetTitleSize(0.045);
  h->GetXaxis()->SetLabelSize(0.045);
  h->GetYaxis()->SetTitleSize(0.045);
  h->GetYaxis()->SetLabelSize(0.045);
  h->GetXaxis()->SetTitleOffset(1.20);
  h->GetYaxis()->SetTitleOffset(1.70);
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>> Main function >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char** argv)
{
  // set user style
  Style();

  // directory with input histograms
  TString baseDir = gHistDir;
  // directory for output plots (must exist)
  TString plotDir = gPlotsDir;
  
  // name patterns for decay channels
  TString suf[3] = {"ee", "mumu", "emu"};
  
  // MC samples for control plots
  // (does not matter which are signal and which are background);
  // each sample can contain multiple subsamples (container of containers, 
  // e.g. this is needed to combine in one entry DY low mass and high mass)
  std::vector<std::vector<TString> > vecMCName; // names (must be consistent with those in ttbarMakeHist.cxx)
  std::vector<int> vecMCColor; // color codes
  std::vector<TString> vecMCtitle; // titles (to be plotted in the legend)
  // they come in the reversed order w.r.t how they will appear in the legend
  // Drell-Yan
  std::vector<TString> DYNames;
  DYNames.push_back("DYlm");
  DYNames.push_back("DYhm");
  vecMCName.push_back(DYNames);
  vecMCColor.push_back(kBlue);
  vecMCtitle.push_back("Z / #gamma*");
  // W+jets
  vecMCName.push_back(std::vector<TString>(1, "Wjets"));
  vecMCColor.push_back(kGreen - 2);
  vecMCtitle.push_back("W+Jets");
  // single top
  vecMCName.push_back(std::vector<TString>(1, "SingleTop"));
  vecMCColor.push_back(kMagenta);
  vecMCtitle.push_back("Single Top");
  // ttbar other
  vecMCName.push_back(std::vector<TString>(1, "SigOther"));
  vecMCColor.push_back(kRed - 7);
  vecMCtitle.push_back("t#bar{t} Other");
  // ttbar signal
  vecMCName.push_back(std::vector<TString>(1, "Sig"));
  vecMCColor.push_back(kRed);
  vecMCtitle.push_back("t#bar{t} Signal");

  // *** make control plots ***
  // container of 2D histograms used to set the plotted range, axis etc.
  std::vector<TH2F*> cpHR;
  // container of variable names
  std::vector<TString> cpVar;
  // pT(top)
  TH2F* hr_cp_ptt = new TH2F("hr_cp_ptt", "", 1, 0, 400, 1, 0, 1000.);
  hr_cp_ptt->GetXaxis()->SetTitle("p_{T}(t) [GeV]");
  hr_cp_ptt->GetYaxis()->SetTitle("Top quarks / 20 GeV");
  SetCPHRange(hr_cp_ptt);
  cpHR.push_back(hr_cp_ptt);
  cpVar.push_back("ptt");
  // pT(ttbar), ttbar = top + antitop
  TH2F* hr_cp_pttt = new TH2F("hr_cp_ptt", "", 1, 0, 300, 1, 0, 1000.);
  hr_cp_pttt->GetXaxis()->SetTitle("p_{T}(t#bar{t}) [GeV]");
  hr_cp_pttt->GetYaxis()->SetTitle("Top quarks / 20 GeV");
  SetCPHRange(hr_cp_pttt);
  cpHR.push_back(hr_cp_pttt);
  cpVar.push_back("pttt");
  // rapidity(top)
  TH2F* hr_cp_yt = new TH2F("hr_cp_yt", "", 1, -2.6, 2.6, 1, 0, 800.);
  hr_cp_yt->GetXaxis()->SetTitle("y(t)");
  hr_cp_yt->GetYaxis()->SetTitle("Top quarks / 0.2");
  SetCPHRange(hr_cp_yt);
  cpHR.push_back(hr_cp_yt);
  cpVar.push_back("yt");
  // rapidity(ttbar)
  TH2F* hr_cp_ytt = new TH2F("hr_cp_ytt", "", 1, -2.6, 2.6, 1, 0, 1000.);
  hr_cp_ytt->GetXaxis()->SetTitle("y(t#bar{t})");
  hr_cp_ytt->GetYaxis()->SetTitle("Top quarks / 0.2");
  SetCPHRange(hr_cp_ytt);
  cpHR.push_back(hr_cp_ytt);
  cpVar.push_back("ytt");
  
  // *** TOP-11-013 Fig. 4 ***
  // (be aware that in the paper plots for pT(top) and rapidity(top) 
  // contains both top and antitop quantities, while here only top is plotted)
  TCanvas* c_cp[4];
  for(int ch = 0; ch < 4; ch++)
  {
    c_cp[ch] = new TCanvas(TString::Format("c%d", ch), "", 800, 800);
    c_cp[ch]->Divide(2, 2);
  }
  for(int v = 0; v < 4; v++)
  {
    TString var = cpVar[v];
    std::vector<TH1D*> hcp;
    hcp.resize(vecMCName.size() + 1);
    // create legend
    TLegend* leg = new TLegend(0.62, 0.62, 0.90, 0.92);
    leg->SetTextSize(0.045);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    // loop over channels
    for (int ch = 1; ch < 4; ch++)
    {
      c_cp[ch]->cd(v + 1);
      cpHR[v]->Draw();
      // MC
      std::vector<TH1D*> vecHMC; // vector of cumulative histograms which are actually to be drawn
      for(int s = 0; s < vecMCName.size(); s++)
      {
        if(s > 0)
          vecHMC.push_back(new TH1D(*vecHMC[s - 1]));
        // open ROOT file, read needed histogram
        TFile* f = TFile::Open(TString::Format("%s/mc%sReco-c%d.root", baseDir.Data(), vecMCName[s][0].Data(), ch));        
        TH1D* h = (TH1D*)f->Get(TString::Format("h_%s", var.Data()));
        // loop over subsamples
        for(int ss = 1; ss < vecMCName[s].size(); ss++)
        {
          TFile* f = TFile::Open(TString::Format("%s/mc%sReco-c%d.root", baseDir.Data(), vecMCName[s][ss].Data(), ch));        
          TH1D* hh = (TH1D*)f->Get(TString::Format("h_%s", var.Data()));
          // plotted histograms are cumulative: each next one = previous one + current one
          h->Add(hh);
        }
        // if this is the first sample, push the current histogram
        if(s == 0)
          vecHMC.push_back(h);
        // otherwise add to the existing histogram
        else
          vecHMC[s]->Add(h);
      }
      // data
      TFile* fData = TFile::Open(TString::Format("%s/data-c%d.root", baseDir.Data(), ch));
      TH1D* hData = (TH1D*)fData->Get(TString::Format("h_%s", var.Data()));
      hData->SetMarkerStyle(20);
      hData->SetMarkerSize(1);
      hData->SetLineColor(1);
      hData->SetMarkerColor(1);
      if(ch == 1)
        leg->AddEntry(hData, "Data", "pe");
      // draw MC
      for(int mc = vecMCName.size() - 1; mc >= 0; mc--)
      {
        vecHMC[mc]->SetFillColor(vecMCColor[mc]);
        vecHMC[mc]->SetLineColor(1);
        vecHMC[mc]->Draw("hist same");
        if(ch == 1)
          leg->AddEntry(vecHMC[mc], vecMCtitle[mc], "f");
        // prepare dilepton combined histograms:
        // for the first channel copy to create a new one
        if(ch == 1)
          hcp[mc] = new TH1D(*vecHMC[mc]);
        // for the rest add to the existing histogram
        else
          hcp[mc]->Add(vecHMC[mc]);
      }
      // draw data
      hData->Draw("e0 same");
      leg->Draw();
      cpHR[v]->Draw("axis same");
      if(ch == 1)
        hcp[hcp.size() - 1] = new TH1D(*hData);
      else
        hcp[hcp.size() - 1]->Add(hData);
    } // end of loop over channels
    // combined
    c_cp[0]->cd(v + 1);
    cpHR[v]->Draw();
    for(int mc = vecMCName.size() - 1; mc >= 0; mc--)
      hcp[mc]->Draw("hist same");
    hcp[hcp.size() - 1]->Draw("e0 same");
    leg->Draw();
    cpHR[v]->Draw("axis same");
  }
  // save plots
  for(int ch = 1; ch < 4; ch++)
  {
    // (channel code is: c1 ee, c2 mumu, c3 emu) 
    c_cp[ch]->SaveAs(TString::Format("%s/cp-c%d.eps", plotDir.Data(), ch));
    c_cp[ch]->SaveAs(TString::Format("%s/cp-c%d.pdf", plotDir.Data(), ch));
  }
  c_cp[0]->SaveAs(TString::Format("%s/cp.eps", plotDir.Data()));
  c_cp[0]->SaveAs(TString::Format("%s/cp.pdf", plotDir.Data()));
  //
  // be aware: there are certainly memory leaks (not removing 
  // dynamically allocated objects), although it does not matter here, 
  // all memory is free when execution finished
  //
  

  // *** make cross sections ***
  // helper object to prepare all input for cross section plotting 
  // (see plots.h for description)
  ZPlotCSInput csIn;
  csIn.Norm = true;
  csIn.Paper = true;
  csIn.baseDir = baseDir;
  csIn.plotDir = plotDir;
  // channels
  // combined
  csIn.VecColor.push_back(1);
  csIn.VecStyle.push_back(20);
  csIn.VecTitle.push_back("Dilepton");
  // ee
  csIn.VecColor.push_back(kBlue);
  csIn.VecStyle.push_back(26);
  csIn.VecTitle.push_back("ee");
  // mumu
  csIn.VecColor.push_back(kGreen + 2);
  csIn.VecStyle.push_back(32);
  csIn.VecTitle.push_back("#mu#mu");
  // emu
  csIn.VecColor.push_back(kRed);
  csIn.VecStyle.push_back(24);
  csIn.VecTitle.push_back("e#mu");
  // MC background
  csIn.VecMCBackgr.push_back("SigOther");
  csIn.VecMCBackgr.push_back("SingleTop");
  csIn.VecMCBackgr.push_back("DYlm");
  csIn.VecMCBackgr.push_back("DYhm");
  csIn.VecMCBackgr.push_back("Wjets");
  // variables
  // pT(top)
  TH2F* hr_cs_ptt = new TH2F("hr_cs_ptt", "", 1, 0, 400, 1, 0, 0.01);
  hr_cs_ptt->GetXaxis()->SetTitle("p_{T}(t) [GeV]");
  hr_cs_ptt->GetYaxis()->SetTitle("#frac{1}{#sigma} #frac{d#sigma}{dp_{T}(t)} [GeV^{-1}]");
  SetCPHRange(hr_cs_ptt);
  csIn.VecHR.push_back(hr_cs_ptt);
  csIn.VecVar.push_back("ptt");
  // rapidity(top)
  TH2F* hr_cs_yt = new TH2F("hr_cs_yt", "", 1, -2.5, 2.5, 1, 0, 0.7);
  hr_cs_yt->GetXaxis()->SetTitle("y(t)");
  hr_cs_yt->GetYaxis()->SetTitle("#frac{1}{#sigma} #frac{d#sigma}{dy(t)}");
  SetCPHRange(hr_cs_yt);
  csIn.VecHR.push_back(hr_cs_yt);
  csIn.VecVar.push_back("yt");
  // pT(ttbar)
  TH2F* hr_cs_pttt = new TH2F("hr_cs_pttt", "", 1, 0, 400, 1, 0, 0.025);
  hr_cs_pttt->GetXaxis()->SetTitle("p_{T}(t#bar{t}) [GeV]");
  hr_cs_pttt->GetYaxis()->SetTitle("#frac{1}{#sigma} #frac{d#sigma}{dp_{T}(t#bar{t})} [GeV^{-1}]");
  SetCPHRange(hr_cs_pttt);
  csIn.VecHR.push_back(hr_cs_pttt);
  csIn.VecVar.push_back("pttt");
  // rapidity(ttbar)
  TH2F* hr_cs_ytt = new TH2F("hr_cs_ytt", "", 1, -2.5, 2.5, 1, 0, 0.8);
  hr_cs_ytt->GetXaxis()->SetTitle("y(t#bar{t})");
  hr_cs_ytt->GetYaxis()->SetTitle("#frac{1}{#sigma} #frac{d#sigma}{dy(t#bar{t})}");
  SetCPHRange(hr_cs_ytt);
  csIn.VecHR.push_back(hr_cs_ytt);
  csIn.VecVar.push_back("ytt");
  // M(ttbar)
  TH2F* hr_cs_mtt = new TH2F("hr_cs_mtt", "", 1, 345, 1600, 1, 1e-6, 0.06);
  hr_cs_mtt->GetXaxis()->SetTitle("M(t#bar{t}) [GeV]");
  hr_cs_mtt->GetYaxis()->SetTitle("#frac{1}{#sigma} #frac{d#sigma}{dM(t#bar{t})} [GeV^{-1}]");
  SetCPHRange(hr_cs_mtt);
  csIn.VecHR.push_back(hr_cs_mtt);
  csIn.VecVar.push_back("mtt");

  // *** TOP-11-013, Fig. 10, and the total x-section from TOP-13-004 ***
  // (see plots.h for description)
  PlotCS(csIn);

  return 0;
}
