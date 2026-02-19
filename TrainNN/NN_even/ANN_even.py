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
parser.add_argument('--PosIntFile', type=str, required=True, help='Path to the +45 CSV file')
parser.add_argument('--NegIntFile', type=str, required=True, help='Path to the -45 CSV file')
parser.add_argument('--EvenIntFile', type=str, required=True, help='Path to the even interference CSV file')

args = parser.parse_args()

# define input names
pos_interference_file = args.PosIntFile
neg_interference_file = args.NegIntFile
even_interference_file = args.EvenIntFile 


# import interference data
df_pos = pd.read_csv(pos_interference_file)
print(df_pos.columns)
df_neg = pd.read_csv(neg_interference_file)
print(df_neg.columns)
df_even = pd.read_csv(even_interference_file)


#scale the data to avoid large values. Ideally replace this with a MinMaxScaler
# only scaled Pt, not sure if other variables need scaling
df_pos['Pt_PiPlus_log']=np.log10(df_pos['Pt_PiPlus'])
df_pos['Pt_PiMinus_log']=np.log10(df_pos['Pt_PiMinus'])
df_neg['Pt_PiPlus_log']=np.log10(df_neg['Pt_PiPlus'])
df_neg['Pt_PiMinus_log']=np.log10(df_neg['Pt_PiMinus'])
df_even['Pt_PiPlus_log']=np.log10(df_even['Pt_PiPlus'])
df_even['Pt_PiMinus_log']=np.log10(df_even['Pt_PiMinus'])

print(df_pos.columns)
print(df_neg.columns)

# determine the input features and target variable
X_pos = df_pos[['Phi_PiPlus', 'Phi_PiMinus', 'Eta_PiPlus', 'Eta_PiMinus', 'Pt_PiPlus_log', 'Pt_PiMinus_log', 'ipX_PiPlus', 'ipY_PiPlus', 'ipZ_PiPlus', 'ipX_PiMinus', 'ipY_PiMinus', 'ipZ_PiMinus']].values
X_neg = df_neg[['Phi_PiPlus', 'Phi_PiMinus', 'Eta_PiPlus', 'Eta_PiMinus', 'Pt_PiPlus_log', 'Pt_PiMinus_log', 'ipX_PiPlus', 'ipY_PiPlus', 'ipZ_PiPlus', 'ipX_PiMinus', 'ipY_PiMinus', 'ipZ_PiMinus']].values
X_even = df_even[['Phi_PiPlus', 'Phi_PiMinus', 'Eta_PiPlus', 'Eta_PiMinus', 'Pt_PiPlus_log', 'Pt_PiMinus_log', 'ipX_PiPlus', 'ipY_PiPlus', 'ipZ_PiPlus', 'ipX_PiMinus', 'ipY_PiMinus', 'ipZ_PiMinus']].values
y_pos = df_pos['Weight'].values
y_neg = df_neg['Weight'].values
y_even = df_even['Weight'].values
y_pos[y_pos>0]=1
y_neg[y_neg<0]=0 
y_even[y_even==0]=2


X_pos = X_pos
X_neg = X_neg
X_even = X_even
names = ['Phi_PiPlus', 'Phi_PiMinus', 'Eta_PiPlus', 'Eta_PiMinus', 'Pt_PiPlus_log', 'Pt_PiMinus_log', 'ipX_PiPlus', 'ipY_PiPlus', 'ipZ_PiPlus', 'ipX_PiMinus', 'ipY_PiMinus', 'ipZ_PiMinus']

#combine the positive and negative samples for multiclass classification
X_tot=np.concatenate((X_pos, X_neg, X_even))
y_tot=np.concatenate((y_pos, y_neg, y_even))


# split data to train/val/test, this is for binary classification
X_train, X_2, y_train, y_2   = train_test_split(X_tot,   y_tot,   test_size=0.2, random_state=101)
X_val, X_test, y_val, y_test = train_test_split(X_2, y_2, test_size=0.3, random_state=101)
# split the data for a multiclass model
#X_train, X_2, y_train, y_2   = train_test_split(X_tot, y_tot,   test_size=0.2, random_state=101)
#X_val, X_test, y_val, y_test = train_test_split(X_2, y_2, test_size=0.3, random_state=101)

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
plt.savefig(f'confusion_matrix.png')  # Save the plot
plt.show()

# plot the loss curve
plt.plot(classifier.loss_curve_)
plt.xlabel('Epoch')
plt.ylabel('loss')
plt.savefig(f'loss_curve.png')  # Save the plot
plt.show()

# perform permutation importance
from sklearn.inspection import permutation_importance
r = permutation_importance(classifier, X_val, y_val,n_repeats=30,random_state=0)
print(r.importances_mean)
#add axis labels
plt.xlabel('importance')
plt.ylabel('feature')
plt.plot(r.importances_mean,names,'bo')
plt.savefig(f'feature_importance.png')  # Save the plot
plt.show()

#wow, I've got to O_NN already!
y_sm = classifier.predict(X_neg)
plt.hist(classifier.predict_proba(X_pos)[:,1]-classifier.predict_proba(X_pos)[:,0],histtype='step',bins=28,range=(-1,1))
plt.hist(classifier.predict_proba(X_neg)[:,1]-classifier.predict_proba(X_neg)[:,0],histtype='step',bins=28,range=(-1,1))
plt.hist(classifier.predict_proba(X_even)[:,1]-classifier.predict_proba(X_even)[:,0],histtype='step',bins=28,range=(-1,1))
plt.xlabel('$O_{NN}$')
plt.ylabel('Weights (arbitrary units)')
plt.legend([f'CP-mix ($\\phi$ = 45°)', 'CP-mix ($\\phi$ = -45°)', 'CP-even ($\\phi$ = 0°)'])
plt.savefig(f'ONN_distribution.png')  # Save the plot
plt.show()

#plot the variable distribution as a simplest cross check
plt.hist(df_pos['SignedAcoplanarity'],histtype='step',bins=28,range=(0,np.pi*2))
plt.hist(df_neg['SignedAcoplanarity'],histtype='step',bins=28, range=(0,np.pi*2))
plt.hist(df_even['SignedAcoplanarity'],histtype='step',bins=28, range=(0,np.pi*2))
#add axis labels to this plot
plt.xlabel('$\\varphi^*_{CP}$')
plt.ylabel('Weights (arbitrary units)')
#add legend to this plot
plt.legend(['CP-mix ($\\phi$ = 45°)','CP-mix ($\\phi$ = -45°)', 'CP-even ($\\phi$ = 0°)']) 
plt.savefig(f'SignedAcoplanarity_Distribution.png')  # Save the plot
plt.show()


# Create a ROOT file to save the histograms
output_file = TFile(f'H_tautau_ONN.root', 'RECREATE')


hist_ONN_pos_interference = TH1D('hist_ONN_pos', 'ONN Positive', 28, -1, 1)
hist_ONN_neg_interference = TH1D('hist_ONN_neg', 'ONN Negative', 28, -1, 1)
hist_ONN_even_interference = TH1D('hist_ONN_even', 'ONN Even', 28, -1, 1)

# Fill the histograms with the data from the DataFrame
for value in zip(classifier.predict_proba(X_pos)[:,1]-classifier.predict_proba(X_pos)[:,0]):
    hist_ONN_pos_interference.Fill(value)

for value in zip(classifier.predict_proba(X_neg)[:,1]-classifier.predict_proba(X_neg)[:,0]):
    hist_ONN_neg_interference.Fill(value)

for value in zip(classifier.predict_proba(X_even)[:,1]-classifier.predict_proba(X_even)[:,0]):
    hist_ONN_even_interference.Fill(value)


# Write the histograms to the ROOT file
hist_ONN_pos_interference.Write()
hist_ONN_neg_interference.Write()
hist_ONN_even_interference.Write()

# Close the ROOT file
output_file.Close()