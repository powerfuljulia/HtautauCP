# $$H\rightarrow\tau\tau$$ CP analysis with NN
---

Basic workflow should be:
- Generate $$H\rightarrow\tau\tau$$ process of interest in with different CP configurations using Pythia:
  - CP-even ($$\phi = 0$$)  
  - CP-odd ($$\phi = 90$$)  
  - Mixed ($$\phi = -45$$ , $$\phi = 45$$)
- Validate samples by plotting signed acoplanarity for each configuration
- Train NN on final state decay products using mixed state samples
  - Test different input variables

Tau leptons decay in many different ways, and the signed acoplanarity is calculated using different methods depending on the decay mode.
https://arxiv.org/pdf/2212.05833 details the different decay modes used in the ATLAS H->tau tau CP analysis, and how the signed acoplanarity is defined for each case

dummy  