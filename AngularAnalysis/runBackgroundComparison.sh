./Analyser --CPState 0 --inFile ../PythiaGeneration/Htautau_CPeven.root --inBackgroundFile Ztautau.root
root -b compare_background.cc'("AnalyserOutput_CPeven.root", "AnalyserOutput_Background.root")'