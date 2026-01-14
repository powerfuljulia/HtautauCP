import argparse

import pandas as pd
import numpy as np
import copy
from ROOT import TH1D, TFile
from ROOT import gROOT

from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, confusion_matrix, ConfusionMatrixDisplay
from sklearn.neural_network import MLPClassifier

import matplotlib.pyplot as plt

import os


# Parse command-line arguments
parser = argparse.ArgumentParser(description='Process input files.')
parser.add_argument('--EFTFile', type=str, required=True, help='Path to the EFT CSV file')
parser.add_argument('--SMFile', type=str, required=True, help='Path to the SM CSV file')
parser.add_argument('--bkgdFile', type=str, required=True, help='Path to the bkgd CSV file')
parser.add_argument('--energy', type=int, required=True, help='energy of the collisions')
parser.add_argument('--withDiTau', action='store_true', help='flag to include DiTau variables')

args = parser.parse_args()

# define input names
interference_file = args.EFTFile
sm_file = args.SMFile
bkgd_file = args.bkgdFile
output_suffix = str(args.energy) + 'TeV'

# Modify output_suffix based on the withDiTau flag
if args.withDiTau:
    output_suffix += '_withDiTau'

# import interference data
df = pd.read_csv(interference_file)
print(df.columns)

#scale the data to avoid large values. Ideally replace this with a MinMaxScaler
df['jet0Pt_log']=np.log10(df['jet0Pt'])
df['jet1Pt_log']=np.log10(df['jet1Pt'])
df['DiTauPt_log']=np.log10(df['DiTauPt'])

#create the input variables (X) and the target (y) for the interference sampl
X=df[['jet0Eta','jet0Phi','jet1Eta','jet1Phi', 'jet0Pt_log','jet1Pt_log']].values
X_withDitau=df[['jet0Eta','jet0Phi','jet1Eta','jet1Phi', 'jet0Pt_log','jet1Pt_log', 'DiTauPt_log', 'DiTauEta', 'DiTauPhi', 'SignedDeltaPhi']].values
y = df['Weight'].values
w = copy.deepcopy(y)
y[y<0]=0
y[y>0]=1


df_sm = pd.read_csv(sm_file)
df_sm['jet0Pt_log']=np.log10(df_sm['jet0Pt'])
df_sm['jet1Pt_log']=np.log10(df_sm['jet1Pt'])
df_sm['DiTauPt_log']=np.log10(df_sm['DiTauPt'])

X_sm = df_sm[['jet0Eta','jet0Phi','jet1Eta','jet1Phi','jet0Pt_log','jet1Pt_log']].values
X_sm_withDiTau = df_sm[['jet0Eta','jet0Phi','jet1Eta','jet1Phi','jet0Pt_log','jet1Pt_log', 'DiTauPt_log', 'DiTauEta', 'DiTauPhi', 'SignedDeltaPhi']].values
y_sm = df_sm['Weight'].values
w_sm = copy.deepcopy(y_sm)
y_sm[y_sm>0]=2 

df_bkgd = pd.read_csv(bkgd_file)
print(df_bkgd.columns)

df_bkgd['jet0Pt_log'] = np.log10(df_bkgd['jet0Pt'])
df_bkgd['jet1Pt_log'] = np.log10(df_bkgd['jet1Pt'])
df_bkgd['DiTauPt_log'] = np.log10(df_bkgd['DiTauPt'])

# Create the input variables (X_extra) and the target (y_extra) for the extra sample
X_bkgd = df_bkgd[['jet0Eta', 'jet0Phi', 'jet1Eta', 'jet1Phi', 'jet0Pt_log', 'jet1Pt_log']].values
X_bkgd_withDiTau = df_bkgd[['jet0Eta', 'jet0Phi', 'jet1Eta', 'jet1Phi', 'jet0Pt_log', 'jet1Pt_log', 'DiTauPt_log', 'DiTauEta', 'DiTauPhi', 'SignedDeltaPhi']].values
y_bkgd = df_bkgd['Weight'].values
w_bkgd = copy.deepcopy(y_bkgd)
y_bkgd[y_bkgd > 0] = 2

print(args.withDiTau)

# Select the appropriate input variables based on the withDiTau flag
if args.withDiTau:
    X = X_withDitau
    X_sm = X_sm_withDiTau
    X_extra = X_bkgd_withDiTau
    names = ['j1y','j1phi','j2y','j2phi','j1pt','j2pt', 'ditaupt', 'ditaue', 'ditauj', 'dphijj']
else:
    X = df[['jet0Eta','jet0Phi','jet1Eta','jet1Phi', 'jet0Pt_log','jet1Pt_log']].values
    X_sm = df_sm[['jet0Eta','jet0Phi','jet1Eta','jet1Phi','jet0Pt_log','jet1Pt_log']].values
    names = ['j1y','j1phi','j2y','j2phi','j1pt','j2pt']

#combine the interference and SM samples for multiclass classification
X_tot=np.concatenate((X,X_sm))
y_tot=np.concatenate((y,y_sm))


# split data to train/val/test, this is for binary classification
#X_train, X_2, y_train, y_2   = train_test_split(X,   y,   test_size=0.2, random_state=101)
#X_val, X_test, y_val, y_test = train_test_split(X_2, y_2, test_size=0.3, random_state=101)
# split the data for a multiclass model
X_train, X_2, y_train, y_2   = train_test_split(X_tot,   y_tot,   test_size=0.2, random_state=101)
X_val, X_test, y_val, y_test = train_test_split(X_2, y_2, test_size=0.3, random_state=101)

# Print the size of X_train
print(f'Size of X_train: {X_train.shape}')
print(f'Size of X_val: {X_val.shape}')
print(f'Size of X_test: {X_test.shape}')

# check that classes are balanced
y_dict={'y_train':y_train, 'y_val':y_val, 'y_test':y_test}
for key,val in y_dict.items():
    print(f'{key} :')
    print(f'\tclass 0 = {100.*(val==0).sum()/len(val):.1f}%')

    
# define the classifier and train it
classifier = MLPClassifier(hidden_layer_sizes=(100,50,20), max_iter=1000,activation = 'relu',solver='adam',random_state=1,batch_size=5000)
classifier.fit(X_train, y_train, )

# evaluate the model metrics
y_pred = classifier.predict(X_val)
from sklearn.metrics import confusion_matrix
cm = confusion_matrix(y_val, y_pred, labels=classifier.classes_)
print(cm)
print(classification_report(y_val,y_pred)) # I get precision, recall, f1-score and support; 
#precision is tp/(tp+fp), recall is tp/(tp+fn), f1-score is 2*(precision*recall)/(precision+recall), support is the number of true instances for each class
disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=classifier.classes_)
disp.plot()
plt.savefig(f'confusion_matrix_{output_suffix}.png')  # Save the plot
plt.show()

# plot the loss curve
plt.plot(classifier.loss_curve_)
plt.xlabel('Epoch')
plt.ylabel('loss')
plt.savefig(f'loss_curve_{output_suffix}.png')  # Save the plot
plt.show()

# perform permutation importance
from sklearn.inspection import permutation_importance
r = permutation_importance(classifier, X_val, y_val,n_repeats=30,random_state=0)
print(r.importances_mean)
#add axis labels
plt.xlabel('importance')
plt.ylabel('feature')
plt.plot(r.importances_mean,names,'bo')
plt.savefig(f'feature_importance_{output_suffix}.png')  # Save the plot
plt.show()

#wow, I've got to O_NN already!
y_sm = classifier.predict(X_sm)
plt.hist(classifier.predict_proba(X)[:,1]-classifier.predict_proba(X)[:,0],weights=w,histtype='step',bins=28,range=(-1,1))
plt.hist(classifier.predict_proba(X_sm)[:,1]-classifier.predict_proba(X_sm)[:,0],weights=w_sm,histtype='step',bins=28,range=(-1,1))
plt.hist(classifier.predict_proba(X_bkgd)[:,1]-classifier.predict_proba(X_bkgd)[:,0],weights=w_bkgd/20,histtype='step',bins=28,range=(-1,1))
plt.xlabel('$O_{NN}$')
plt.ylabel('Weights (arbitrary units)')
plt.legend(['Interference','SM','Bkgd/20'])
plt.savefig(f'ONN_distribution_{output_suffix}.png')  # Save the plot
plt.show()

#plot the dphijj distribution as a simplest cross check
plt.hist(df['SignedDeltaPhi'],weights=w,histtype='step',bins=28,range=(-1*np.pi,np.pi))
plt.hist(df_sm['SignedDeltaPhi'],weights=w_sm,histtype='step',bins=28, range=(-1*np.pi,np.pi))
plt.hist(df_bkgd['SignedDeltaPhi'],weights=w_bkgd/20,histtype='step',bins=28, range=(-1*np.pi,np.pi))
#add axis labels to this plot
plt.xlabel('$\Delta\phi_{jj}$')
plt.ylabel('Weights (arbitrary units)')
#add legend to this plot
plt.legend(['Interference','SM', 'Bkgd/20'])
plt.savefig(f'dphijj_distribution_{output_suffix}.png')  # Save the plot
plt.show()

# Create a ROOT file to save the histograms
output_file = TFile(f'VBFH_tautau_ONN_{output_suffix}.root', 'RECREATE')

# Create a TH1D histogram for the 'SignedDeltaPhi' column
hist_ONN_interference = TH1D('hist_ONN_EFT', 'ONN EFT', 28, -1, 1)
hist_ONN_sm = TH1D('hist_ONN_sm', 'ONN SM', 28, -1, 1)
hist_ONN_bkgd = TH1D('hist_ONN_bkgd', 'ONN bkgd', 28, -1, 1)

# Fill the histograms with the data from the DataFrame
for value, weight in zip(classifier.predict_proba(X)[:,1]-classifier.predict_proba(X)[:,0], w):
    hist_ONN_interference.Fill(value, weight)

for value, weight in zip(classifier.predict_proba(X_sm)[:,1]-classifier.predict_proba(X_sm)[:,0], w_sm):
    hist_ONN_sm.Fill(value, weight)

for value, weight in zip(classifier.predict_proba(X_bkgd)[:,1]-classifier.predict_proba(X_bkgd)[:,0], w_bkgd):
    hist_ONN_bkgd.Fill(value, weight)


# Write the histograms to the ROOT file
hist_ONN_interference.Write()
hist_ONN_sm.Write()
hist_ONN_bkgd.Write()

# Close the ROOT file
output_file.Close()