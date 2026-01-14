# $H\rightarrow\tau\tau$ CP analysis with NN
---

Basic workflow should be:
- Generate $H\rightarrow\tau\tau$ process of interest in with different CP configurations using Pythia:
  - CP-even ($\phi = 0$)  
  - CP-odd ($\phi = 90$)  
  - Mixed ($\phi = -45$ , $\phi = 45$)
- Validate samples by plotting signed acoplanarity for each configuration
- Train NN on final state decay products using mixed state samples
  - Test different input variables

Tau leptons decay in many different ways, and the signed acoplanarity is calculated using different methods depending on the decay mode.
[This ATLAS paper](https://arxiv.org/pdf/2212.05833) details the different decay modes used in the $H\rightarrow\tau\tau$  CP analysis, and how the signed acoplanarity is defined for each case

## Setup
  
You can clone this repository using git via https or ssh, i.e.
```bash
git clone https://gitlab.cern.ch/jusilva/htautaucp_project.git
```

Before you start doing anything else, you need to make sure the following are installed on your machine: python 3.7 (or later), gcc/gfortran 4.6 (or later), and the CERN ROOT libraries. For running the analysis steps later, you will also need scikit-hep installed. If these packages are not installed on your machine, an easy way to proceed is to install conda/mamba (https://github.com/conda-forge/miniforge#mambaforge) and create a conda environment using the environment.yml file provided in this package, i.e.

```bash
curl -L -O "https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-$(uname)-$(uname -m).sh"
bash Miniforge3-$(uname)-$(uname -m).sh -b
conda env create -f htautaucp_project/environment.yml
conda activate CP_NN
```

Then, you can go into the directory and install/build Pythia:

```bash
cd htautaucp_project
source installPythia.sh
```

Pythia allows you to generate particle physics processes at truth level, but it does not generate the detector response. Down the line we could want to generate also the response of the ATLAS detector, and for that you could install/build Delphes (but not really needed to start with).

```bash
cd htautaucp_project
source installDelphes.sh
```

## Pythia Generation

You can now generate some particle interactions! For that we use Pythia, whose [manual](https://pythia.org/latest-manual/Welcome.html) you can take a look at to learn a bit more about what is going on. In this case we are interested in an LHC enviroment, so pp collisions at around 13 TeV centre of mass energy; and on Higgs production, and Higgs decays to tau leptons.
Most of the relevant settings for the process we are interested in simulating are defined [here](https://pythia.org/latest-manual/HiggsProcesses.html).
Higgs bosons get produced through different processes, most often through the fusion of two gluons (gluon-gluon fusion or ggF). Figure 11.1 of the [PDG review on the Higgs boson](https://pdg.lbl.gov/2023/reviews/rpp2023-rev-higgs-boson.pdf) shows different Higgs production mechanisms. Higgs bosons also decay in many different ways, namely pairs of fermions-antifermions (Yukawa coupling), like is the case for $H\rightarrow\tau^{+}\tau^{-}$. Table 11.3 of the same PDG review lists the rates of different Higgs decay processes.
We are interested in probing the CP properties of the Higgs interactions to tau leptons. For that it is useful to generate the $H\rightarrow\tau^{+}\tau^{-}$ process under different CP hypothesis. Different CP admixtures in the Higgs-tau couplings can be set using the 'HiggsH1:parity' and 'HiggsH1:phiParity' parameters also described [here](https://pythia.org/latest-manual/HiggsProcesses.html).

In `PythiaGeneration` you have an example of generation of $H\rightarrow\tau^{+}\tau^{-}$, under the CP-even hypothesis. The tau leptons are also forced to decay by $\tau\rightarrow\pi\nu$. You can generate some events by doing:

```bash
cd PythiaGeneration
make
./Htautau
```

You should have as output a `Htautau.root` file, which is a ntuple, or ROOT tree, which is a very commonly used format for particle physics events which is widely used by physicists at CERN.

## Analysis

A preliminary setup to do some analysis with the events saved in the root file can be found in the `AngularAnalysis` directory. This is C++ code and uses ROOT to open the file, loop over the events, build some `TLorentzVectors` of the truth particles, fill some histograms and make some plots. You can find some tutorials to help you get started with ROOT [here](https://root.cern/doc/v638/group__Tutorials.html). We can modify this code to do some of the angular validation we are interested in (this still needs to be added). To run the example as is, you can do:

```bash
cd AngularAnalysis
make
./Analyser
```

This should give you 2 plots, one of the truth level Higgs Mass, the other of the truth level di-tau visible mass. The distinction comes from the fact that it is not possible to detect the neutrinos at the LHC with a detector like ATLAS, so tau leptons are only partially reconstructed - only the 'visible' part gets reconstructed - in this case the charged pions resulting from the decay.
