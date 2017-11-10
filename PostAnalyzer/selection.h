// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>> Helper for ttbar event selection >>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Consult analysis documentation (papers, description-ttbar.pdf) for 
// better description of applied cuts etc.

// additional files from this analysis 
#include "tree.h"
// C++ library or ROOT header files
#include <TLorentzVector.h>

// constants: electron and muon masses
// (not the best practice to make them global variables, be aware)
const double massEl = 0.000511;
const double massMu = 0.105658;

// Routine for electron selection
// Arguments:
//   const ZTree* preselTree: input tree (see tree.h), GetEntry() should be done already
//   const int el: electron candidate
// Returns true for selected electron, false otherwise.
// See tree.h for ZTree variables description.
bool SelectEl(const ZTree* preselTree, const int el)
{
  // require pT(e) > 20 GeV
  if(TMath::Abs(preselTree->elPt[el]) < 20.0)
    return false;
  // require |eta(e)| > 2.4
  if(TMath::Abs(preselTree->elEta[el]) > 2.4)
    return false;
  // require isolation (delta_R = 0.3) > 0.17
  if(preselTree->elIso03[el] > 0.17)
    return false;
  // require no missing hits
  if(preselTree->elMissHits[el] > 0)
    return false;
  // cuts on conversiopn variables not applied: study them if you want
  //if(preselTree->elConvDist[el] < 0.02 && preselTree->elConvDcot[el] < 0.02 && preselTree->elConvDist[el] >= 0.0 && preselTree->elConvDcot[el] >=0.0)
  //  return false;
  // all cuts passed: return true
  return true;
}

// Routine for muon selection
// Arguments:
//   const ZTree* preselTree: input tree (see tree.h), GetEntry() should be done already
//   const int el: muon candidate
// Returns true for selected muon, false otherwise.
// See tree.h for ZTree variables description.
bool SelectMu(const ZTree* preselTree, const int mu)
{
  // require pT(mu) > 20 GeV
  if(TMath::Abs(preselTree->muPt[mu]) < 20.0)
    return false;
  // require |eta(mu)| > 2.4
  if(TMath::Abs(preselTree->muEta[mu]) > 2.4)
    return false;
  // require isolation (delta_R = 0.3) > 0.20
  if(preselTree->muIso03[mu] > 0.20)
    return false;
  // require at least 11 tracker hits and at least 1 pixel hit
  if(preselTree->muHitsValid[mu] < 12 || preselTree->muHitsPixel[mu] < 2)
    return false;
  // the transverse impact parameter of the muon w.r.t the primary vertex should be smaller than 0.2 mm, 
  // the corresponding distance in z should be smaller than 5 mm and the global track should have chi2/dof > 10
  if(preselTree->muDistPV0[mu] > 0.02 || preselTree->muDistPVz[mu] > 0.5 || preselTree->muTrackChi2NDOF[mu] > 10)
    return false;
  // all cuts passed: return true
  return true;
}

// routine for electron-muon pair selection
// (select best e-mu pair in the event, with highest pT)
// Arguments:
//   const ZTree* preselTree: input tree (see tree.h), GetEntry() should be done already
//   TLorentzVector& vecLepM: selected lepton- (output)
//   TLorentzVector& vecLepP: selected lepton+ (output)
//   double& maxPtDiLep: transverse momentum of the selected dilepton pair (output)
// If no dilepton pair is selected, maxPtDiLep remains unchanged 
// (not the best practice to make them global variables, be aware)
void SelectDilepEMu(const ZTree* preselTree, TLorentzVector& vecLepM, TLorentzVector& vecLepP, double& maxPtDiLep)
{
  // loop over electrons
  for(int el = 0; el < preselTree->Nel; el++)
  {
    // electron selection
    if(!SelectEl(preselTree, el))
      continue;
    TLorentzVector thisEl;
    thisEl.SetPtEtaPhiM(TMath::Abs(preselTree->elPt[el]), preselTree->elEta[el], preselTree->elPhi[el], massEl);
    // loop over muons
    for(int mu = 0; mu < preselTree->Nmu; mu++)
    {
      // require opposite signs
      if(preselTree->elPt[el] * preselTree->muPt[mu] > 0)
        continue;
      // muon selection
      if(!SelectMu(preselTree, mu))
        continue;
      TLorentzVector thisMu;
      thisMu.SetPtEtaPhiM(TMath::Abs(preselTree->muPt[mu]), preselTree->muEta[mu], preselTree->muPhi[mu], massMu);
      // require dilepton mass greater than 12 GeV
      TLorentzVector vecDiLep = thisEl + thisMu;
      if(vecDiLep.M() < 12.0)
        continue;
      // select pair with highest transverse momentum
      double sumPt = thisMu.Pt() + thisEl.Pt();
      if(sumPt < maxPtDiLep)
        continue;
      maxPtDiLep = sumPt;
      // assign el and mu momenta to output l+ and l- vectors
      vecLepM = (preselTree->elPt[el] < 0) ? thisEl : thisMu;
      vecLepP = (preselTree->elPt[el] < 0) ? thisMu : thisEl;
    }
  }
}

// routine for electron-electron pair selection
// (select best e-e pair in the event, with highest pT)
// Arguments:
//   const ZTree* preselTree: input tree (see tree.h), GetEntry() should be done already
//   TLorentzVector& vecLepM: selected lepton- (output)
//   TLorentzVector& vecLepP: selected lepton+ (output)
//   double& maxPtDiLep: transverse momentum of the selected dilepton pair (output)
// If no dilepton pair is selected, maxPtDiLep remains unchanged 
// (not the best practice to make them global variables, be aware)
void SelectDilepEE(const ZTree* preselTree, TLorentzVector& vecLepM, TLorentzVector& vecLepP, double& maxPtDiLep)
{
  // loop over 1st electron
  for(int el1 = 0; el1 < preselTree->Nel; el1++)
  {
    // electron selection
    if(!SelectEl(preselTree, el1))
      continue;
    TLorentzVector thisEl1;
    thisEl1.SetPtEtaPhiM(TMath::Abs(preselTree->elPt[el1]), preselTree->elEta[el1], preselTree->elPhi[el1], massEl);
    // loop over 2nd electron
    for(int el2 = el1 + 1; el2 < preselTree->Nel; el2++)
    {
      // require opposite signs
      if(preselTree->elPt[el1] * preselTree->elPt[el2] > 0)
        continue;
      // electron selection
      if(!SelectEl(preselTree, el2))
        continue;
      TLorentzVector thisEl2;
      thisEl2.SetPtEtaPhiM(TMath::Abs(preselTree->elPt[el2]), preselTree->elEta[el2], preselTree->elPhi[el2], massEl);
      // require dilepton mass greater than 12 GeV
      TLorentzVector vecDiLep = thisEl1 + thisEl2;
      if(vecDiLep.M() < 12.0)
        continue;
      // this is additional invariant mass requirement for ee and mumu
      // (to supress Drell-Yan background)
      if(vecDiLep.M() > 76.0 && vecDiLep.M() < 106.0)
        continue;
      // select pair with highest transverse momenta
      double sumPt = thisEl1.Pt() + thisEl2.Pt();
      if(sumPt < maxPtDiLep)
        continue;
      maxPtDiLep = sumPt;
      // assign el and mu momenta to output l+ and l- vectors
      vecLepM = (preselTree->elPt[el1] < 0) ? thisEl1 : thisEl2;
      vecLepP = (preselTree->elPt[el1] < 0) ? thisEl2 : thisEl1;
    }
  }
}

// routine for muon-muon pair selection
// (select best mu-mu pair in the event, with highest pT)
// Arguments:
//   const ZTree* preselTree: input tree (see tree.h), GetEntry() should be done already
//   TLorentzVector& vecLepM: selected lepton- (output)
//   TLorentzVector& vecLepP: selected lepton+ (output)
//   double& maxPtDiLep: transverse momentum of the selected dilepton pair (output)
// If no dilepton pair is selected, maxPtDiLep remains unchanged 
// (not the best practice to make them global variables, be aware)
void SelectDilepMuMu(const ZTree* preselTree, TLorentzVector& vecLepM, TLorentzVector& vecLepP, double& maxPtDiLep)
{
  // loop over 1st muon
  for(int mu1 = 0; mu1 < preselTree->Nmu; mu1++)
  {
    // muon selection
    if(!SelectMu(preselTree, mu1))
      continue;
    TLorentzVector thisMu1;
    thisMu1.SetPtEtaPhiM(TMath::Abs(preselTree->muPt[mu1]), preselTree->muEta[mu1], preselTree->muPhi[mu1], massMu);
    // loop over 2nd muon
    for(int mu2 = mu1 + 1; mu2 < preselTree->Nmu; mu2++)
    {
      // require opposite signs
      if(preselTree->muPt[mu1] * preselTree->muPt[mu2] > 0)
        continue;
      // muon selection
      if(!SelectMu(preselTree, mu2))
        continue;
      TLorentzVector thisMu2;
      thisMu2.SetPtEtaPhiM(TMath::Abs(preselTree->muPt[mu2]), preselTree->muEta[mu2], preselTree->muPhi[mu2], massMu);
      // require dilepton mass greater than 12 GeV
      TLorentzVector vecDiLep = thisMu1 + thisMu2;
      if(vecDiLep.M() < 12.0)
        continue;
      // this is additional invariant mass requirement for ee and mumu
      // (to supress Drell-Yan background)
      if(vecDiLep.M() > 76.0 && vecDiLep.M() < 106.0)
        continue;
      // select pair with highest transverse momenta
      double sumPt = thisMu1.Pt() + thisMu2.Pt();
      if(sumPt < maxPtDiLep)
        continue;
      maxPtDiLep = sumPt;
      // assign el and mu momenta to output l+ and l- vectors
      vecLepM = (preselTree->muPt[mu1] < 0) ? thisMu1 : thisMu2;
      vecLepP = (preselTree->muPt[mu1] < 0) ? thisMu2 : thisMu1;
    }
  }
}
