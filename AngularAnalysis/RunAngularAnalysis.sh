./Analyser --CPState 0 --inFile ../PythiaGeneration/Htautau.root
./Analyser --CPState 1 --inFile ../PythiaGeneration/Htautau_CPodd.root
./Analyser --CPState 2  --phi 45 --inFile ../PythiaGeneration/Htautau_CPmix45.root
./Analyser --CPState 2  --phi -45 --inFile ../PythiaGeneration/Htautau_CPmix-45.root
rm AnalyserOutput_CP.root
hadd AnalyserOutput_CP.root AnalyserOutput_CP*.root
root -b compare_CPhypos.cc'("AnalyserOutput_CP.root")'
