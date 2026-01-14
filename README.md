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
curl -L -O "https://github.com/conda-forge/miniforge/releases/latest/download/Mambaforge-$(uname)-$(uname -m).sh"
bash Mambaforge-$(uname)-$(uname -m).sh
conda env create -f environment.yml
conda init
conda activate NN-CPV2
```



