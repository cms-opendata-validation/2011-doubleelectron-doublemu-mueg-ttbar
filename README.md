# CMS measurements of top quark pair production in dilepton channel at 7 TeV

Relevant CMS publications:
 * normalised cross sections: EPJ C73 (2013) 2339 [arXiv:1211.2220, TOP-11-013]
 * total cross section: JHEP 1608 (2016) 029 [arXiv:1603.02303,TOP-13-004]

Further relevant information can be found also in DESY-THESIS-2012-037
For the general description of the analysis see also attached description-ttbar.pdf

There are two parts in this analysis:
 * Analyzer: ntuple production, requires CMSSW (the instructions assume that you will work on a VM properly contextualized for CMS, available from http://opendata.cern.ch/VM/CMS) and network connection; takes ~ 2 weeks to process the full data + MC samples and ~ 3GB free space for the produced ntuples
 * PostAnalyzer: ntuple processing, produces final numbers and plots, standalone code (requires only gcc and ROOT); takes about 5 minutes

## Instructions how to run the analysis

First you need to create the working area (this step is only needed the first time you setup this program). You can create the working area for this analysis on the VM which has other instances of CMSSW, just keep them in different directories.
```
mkdir WorkingArea
cd ./WorkingArea
cmsrel CMSSW_5_3_32
cd ./CMSSW_5_3_32/src
cmsenv
git clone git://github.com/cms-opendata-validation/2011-doubleelectron-doublemu-mueg-ttbar.git
scram b
cd 2011-doubleelectron-doublemu-mueg-ttbar/Analyzer
ln -sf /cvmfs/cms-opendata-conddb.cern.ch/FT_53_LV5_AN1_RUNA FT_53_LV5_AN1
ln -sf /cvmfs/cms-opendata-conddb.cern.ch/START53_LV6A1 START53_LV6A1
```
(no need to download data/MC input file lists and JSON: provided with the code)

Also the code in PostAnalyzer should be compiled:
```
cd PostAnalyzer
./compile.sh
```

## Running the analysis
Generally, the analysis steps are:
 * run Analyzer/run.sh (look inside first), this processes AOD files (CMS data stored at CERN server, several TB) and produces plain ROOT ntuple files (~3GB), takes ~ 2 weeks, extensive network access
 * move produced ntuples to PostAnalyzer directories (this step is manual on purpose, in order not to overwrite accidentally ntuples produced taking long time etc.)
 * run PostAnalyzer/ttbarMakeHist to process ROOT ntuples to create histograms (~5 mins)
 * run PostAnalyzer/ttbarMakePlots to produce final plots from created histograms (few seconds)

Further description of these steps you can find Analyzer/README.txt and Postanalyzer/README.txt
