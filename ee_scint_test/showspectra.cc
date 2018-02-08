#include	 <iostream>
#include	 <fstream>
#include	 <TGaxis.h>
#include	 <sstream>
#include	 <TGraph.h>
#include	 <TGraphErrors.h>
#include	 <TCanvas.h>
#include	 <TApplication.h>
#include	 <stdlib.h>
#include	 <TF1.h>
#include	 <TH1.h>
#include	 <TProfile.h>
#include	 <TObjArray.h>
#include	 <TStyle.h>
#include	 <TMarker.h>
#include	 <math.h>
#include	 <TStyle.h>
#include	 <TPaveStats.h>
#include	 <TPaveText.h>
#include	 <vector>
#include	 <string.h>
#include	 <fstream>
#include	 <TROOT.h>
#include	 <TFile.h>
#include	 <TLegend.h>
#include         <TLegendEntry.h>
#include	 <time.h>
#include	 <TH2F.h>
#include         <assert.h>
#include	 <string>
#include	 <TRandom.h>
#include 	 <TTree.h>
#include	 <TChain.h>
#include	 <TVector.h>
#include	 <vector>
#include	 <utility>
#include	 <TLeaf.h>
#include	 <math.h>

#define		ENERGY		"321kev"

using		 namespace std;

void PlotHist(TCanvas *C, int styleIndex, int canvasIndex, TH1D *hPlot, TString title, TString xtitle, TString ytitle, TString command);
TH1D* FillHistogramFromFile(TString fileName, TString histTitle);

// Used for visualization, keeps the graph on screen.
TApplication plot_program("FADC_readin",0,0,0,0);

//-------------------------------------------------//
//------------ Start of Program -------------------//
//-------------------------------------------------//

int main(int argc, char* argv[])
{
  // creating canvas for plotting
  TCanvas *C = new TCanvas("canvas", "canvas", 800, 400);
//  C->Divide(2,1);

  TH1D *hpositron = FillHistogramFromFile(Form("positron_%s.txt", ENERGY), Form("positron %s", ENERGY));
  TH1D *helectron = FillHistogramFromFile(Form("electron_%s.txt", ENERGY), Form("electron %s", ENERGY));

  cout << "Max e+ bin contents: " << hpositron->GetBinContent(hpositron->GetMaximumBin()) + hpositron->GetBinContent(hpositron->GetMaximumBin() - 1) << endl;
  cout << "Max e- bin contents: " << helectron->GetBinContent(helectron->GetMaximumBin()) + helectron->GetBinContent(helectron->GetMaximumBin() - 1) << endl;

  PlotHist(C, 1, 1, hpositron, Form("GEANT4 e+e- comparison at %s", ENERGY), "Energy (KeV)", "Counts", "");
  PlotHist(C, 2, 1, helectron, Form("GEANT4 e+e- comparison at %s", ENERGY), "Energy (KeV)", "Counts", "SAME");

  TLegend *l = new TLegend(0.2,0.7,0.3,0.9);
  l->AddEntry(hpositron, "e+", "f");
  l->AddEntry(helectron, "e-", "f");
  l->Draw();

  // Save our plot and print it out as a pdf.
//  C -> Print(Form("G4_ee_spectra_%s.pdf", ENERGY));
  cout << "-------------- End of Program ---------------" << endl;
  plot_program.Run();

  return 0;
}

TH1D* FillHistogramFromFile(TString fileName, TString histTitle)
{
  TH1D *h = new TH1D("main", histTitle.Data(), 100, 0, 1000);

  string name;
  double energy, energyQ, hitTime;
  int nTracks;
  double xEWeightedPos, yEWeightedPos, zEWeightedPos;
  double xE2WeightedPos, yE2WeightedPos, zE2WeightedPos;

  string buf;
  ifstream infile;
  cout << "The file being opened is: " << fileName << endl;
  infile.open(fileName);

  if(!infile.is_open())
    cout << "Problem opening " << fileName << endl;


  while(getline(infile, buf))
  {
    istringstream bufstream(buf);
    if(!bufstream.eof())
    {
      bufstream >> name >> energy >> energyQ >> hitTime >> nTracks
		>> xEWeightedPos >> yEWeightedPos >> zEWeightedPos
		>> xE2WeightedPos >> yE2WeightedPos >> zE2WeightedPos;


      h->Fill(energy);

    }
  }

  infile.close();

  return h;

}


void PlotHist(TCanvas *C, int styleIndex, int canvasIndex, TH1D *hPlot, TString title, TString xtitle, TString ytitle, TString command)
{
  C -> cd(canvasIndex);
  gPad->SetLogy();
  hPlot -> SetTitle(title);
  hPlot -> GetXaxis() -> SetTitle(xtitle);
  hPlot -> GetXaxis() -> CenterTitle();
  hPlot -> GetYaxis() -> SetTitle(ytitle);
  hPlot -> GetYaxis() -> CenterTitle();
//  hPlot -> GetYaxis() -> SetRangeUser(0, 0.000004);

  if(styleIndex == 1)
  {
    hPlot -> SetFillColor(46);
    hPlot -> SetFillStyle(3004);
//    hPlot -> SetFillStyle(3001);
  }
  if(styleIndex == 2)
  {
    hPlot -> SetFillColor(38);
    hPlot -> SetFillStyle(3005);
//    hPlot -> SetFillStyle(3001);
  }
  if(styleIndex == 3)
  {
    hPlot -> SetFillColor(29);
//    hPlot -> SetFillStyle(3005);
    hPlot -> SetFillStyle(3001);
  }

  hPlot -> Draw(command);
  C -> Update();
}
