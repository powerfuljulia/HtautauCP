import argparse

import pandas as pd
import numpy as np
import copy
from ROOT import TH1D, TFile
from ROOT import gROOT

from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, confusion_matrix, ConfusionMatrixDisplay
from sklearn.neural_network import MLPClassifier
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import GridSearchCV

import matplotlib.pyplot as plt

import os


# Parse command-line arguments
parser = argparse.ArgumentParser(description='Process input files.')
parser.add_argument('--phiVals', type=str, required=True, help='Comma-separated list of phi values to process (eg. +45,-45)')
parser.add_argument('--InputFileList', type=str, required=False, default='file_list.txt', help='Path to the text file containing list of phi values and file paths')

args = parser.parse_args()

# read input file list
input_file_list = args.InputFileList

# get phi values from command line
selected_phi_values = args.phiVals.split(',')

# create a dictionary to store the file paths and corresponding phi values
dataframes = {}

# initialise scaler
scaler = StandardScaler()

# read the input file list and load data
with open(input_file_list, 'r') as file:
    for line in file:
        phi, file_path = line.strip().split(',')

        # strip any whitespace from phi and file_path
        phi = phi.strip()
        file_path = file_path.strip()
        # check if current phi value is in selected list
        if phi in selected_phi_values:
            print(f'processing phi={phi}: {file_path}')
            # load csv file into dataframe
            df = pd.read_csv(file_path)

            # store dataframe in dictionary with phi as key
            dataframes[phi] = df

        else:
            print(f'Phi value {phi} not in selected list, skipping file {file_path}')

# define input features and target variable dynamically based on the columns in the dataframes
X = {}
y = {}

# create mapping for phi values
phi_to_weight = {phi: idx for idx, phi in enumerate(selected_phi_values)}

# loop through dataframes
for phi, df in dataframes.items():
    X[phi] = df[['SignedAcoplanarity'
                 #, 'Phi_PiPlus', 'Phi_PiMinus', 'Eta_PiPlus', 'Eta_PiMinus', 'Pt_PiPlus', 'Pt_PiMinus'
                 #, 'ipX_PiPlus', 'ipY_PiPlus', 'ipZ_PiPlus', 'ipX_PiMinus', 'ipY_PiMinus', 'ipZ_PiMinus'
                 ]].values
    weight = phi_to_weight[phi]
    y[phi] = np.full(df['Weight'].shape, weight)


    #df['Pt_PiPlus_log']=np.log10(df['Pt_PiPlus'])
    #df['Pt_PiMinus_log']=np.log10(df['Pt_PiMinus'])

# dictionary to store scaled data
X_scaled = {}

# loop through all phi values and scale input variables
for phi, X_phi in X.items():
    X_scaled[phi] = scaler.fit_transform(X_phi)


names = ['SignedAcoplanarity'
        #, 'Phi_PiPlus', 'Phi_PiMinus', 'Eta_PiPlus', 'Eta_PiMinus', 'Pt_PiPlus', 'Pt_PiMinus'
        # , 'ipX_PiPlus', 'ipY_PiPlus', 'ipZ_PiPlus', 'ipX_PiMinus', 'ipY_PiMinus', 'ipZ_PiMinus'
         ]

#combine the positive and negative samples for multiclass classification
X_tot=np.concatenate((X_scaled['+45'], X_scaled['-45']))
y_tot=np.concatenate((y['+45'], y['-45']))


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

# define parameter grid for hyperparameter tuning

param_grid = {
    'hidden_layer_sizes': [(200,100,50,20), (500,200,100,50), (500,200,100,20)],
    'activation': ['relu'],
    'solver': ['adam'],
    'batch_size': [5000],
    'max_iter': [500],
    'alpha': [0.001],  # L2 regularization parameter
}

    
# define the classifier and train it


# add early stopping to monitor the validation loss and prevent overfitting
classifier = MLPClassifier(hidden_layer_sizes=(100,50,20), max_iter=1000,activation = 'relu',solver='adam',random_state=1,batch_size=5000,
                           #early_stopping=True, # stops training when validation score is not improving
                           #validation_fraction=0., # fraction of training data to set aside as validation set for early stopping
                           #n_iter_no_change=20
                           )
                            # stop if no improvement in validation score for 10 iterations
classifier.fit(X_train, y_train, )
"""

# define the classifier and perform hyperparameter tuning using GridSearchCV
# add early stopping to monitor the validation loss and prevent overfitting
classifier = MLPClassifier(random_state=1, early_stopping=True, validation_fraction=0.2, n_iter_no_change=10)
grid_search = GridSearchCV(estimator=classifier, param_grid=param_grid, cv=3, n_jobs=1, verbose=2)
grid_search.fit(X_train, y_train)

# print scores of each hyperparameter combination
print("\nGrid Search Scores:")
for mean, params in zip(grid_search.cv_results_['mean_test_score'], grid_search.cv_results_['params']):
    print(f"Mean Test Score: {mean:.4f} for Hyperparameters: {params}")



# print the best hyperparameters and best score
print("Best Hyperparameters:", grid_search.best_params_)
print("Best Score:", grid_search.best_score_)

# evaluate best model on validation set
classifier = grid_search.best_estimator_
"""

# training accuracy
train_accuracy = classifier.score(X_train, y_train)
print(f'Training Accuracy: {train_accuracy:.4f}')

# validation accuracy
val_accuracy = classifier.score(X_val, y_val)
print(f'Validation Accuracy: {val_accuracy:.4f}')

# compare training and validation accuracy
if train_accuracy > val_accuracy:
    print('Training accuracy is higher than validation accuracy, model may be overfitting.')
else:
    print('Training and validation accuracy are comparable')


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
plt.figure(figsize=(11,6))
#add axis labels
plt.xlabel('importance')
plt.ylabel('feature')
plt.plot(r.importances_mean,names,'bo')
plt.savefig(f'feature_importance.png')  # Save the plot
plt.show()

#wow, I've got to O_NN already!
#y_sm = classifier.predict(X_neg)
legend_labels = []
# dynamically plot histograms for all phi values
for phi, X_phi_scaled in X_scaled.items():
    if phi =='0':
        label = f'CP-even ($\\phi$ = 0°)'
    elif phi =='90':
        label = f'CP-odd ($\\phi$ = 90°)'
    else:
        label = f'CP-mix ($\\phi$ = {phi}°)'

    legend_labels.append(label)

    plt.hist(classifier.predict_proba(X_phi_scaled)[:,0]-classifier.predict_proba(X_phi_scaled)[:,1],histtype='step',bins=28,range=(-1,1))

plt.xlabel('$O_{NN}$')
plt.ylabel('Weights (arbitrary units)')
plt.legend(legend_labels)
plt.savefig(f'ONN_distribution.png')  # Save the plot
plt.show()

"""
for phi, X_phi_scaled in X_scaled.items():
    plt.hist(classifier.predict_proba(X_phi_scaled)[:,0]-classifier.predict_proba(X_phi_scaled)[:,1],histtype='step',bins=28,range=(-1,-0.99999))

plt.xlabel('$O_{NN}$')
plt.ylabel('Weights (arbitrary units)')
plt.legend(legend_labels)
plt.savefig(f'ONN_distribution1.png')  # Save the plot
plt.show()

for phi, X_phi_scaled in X_scaled.items():
    plt.hist(classifier.predict_proba(X_phi_scaled)[:,0]-classifier.predict_proba(X_phi_scaled)[:,1],histtype='step',bins=28,range=(0.99999,1))

plt.xlabel('$O_{NN}$')
plt.ylabel('Weights (arbitrary units)')
plt.legend(legend_labels)
plt.savefig(f'ONN_distribution2.png')  # Save the plot
plt.show()
"""

# exclude weight column from df.columns
filtered_columns = [col for col in df.columns if col != 'Weight']

# plot distributions of all input variables before and after scaling
for variable in filtered_columns:

    combined_unscaled_data = []

    combined_scaled_data = []


    for phi, X_phi in X.items():
        # extract unscaled data from column corresponding to variable and append to combined_unscaled_data
        combined_unscaled_data.append(X_phi[:, filtered_columns.index(variable)])

    for phi, X_phi_scaled in X_scaled.items():
        # extract scaled data from column corresponding to variable and append to combined_scaled_data
        combined_scaled_data.append(X_phi_scaled[:, filtered_columns.index(variable)])
        

    # plot unscaled distribution of this variable
    plt.hist(combined_unscaled_data, bins=28, histtype='step')
    plt.xlabel(variable)
    plt.ylabel('Weights (arbitrary units)')
    plt.legend(legend_labels)
    plt.title(f'Unscaled Distribution of {variable}')
    plt.savefig(f'Unscaled_{variable}_distribution.png')
    plt.show()

    # plot the distribution of this scaled variable for all phi values
    plt.hist(combined_scaled_data, bins=28, histtype='step')
    plt.xlabel(f'Scaled {variable}')
    plt.ylabel('Weights (arbitrary units)')
    plt.legend(legend_labels)
    plt.title(f'Scaled Distribution of {variable}')
    plt.savefig(f'Scaled_{variable}_distribution.png')
    plt.show()


#plot the variable distribution as a simplest cross check
for phi, df in dataframes.items():
    plt.hist(df['SignedAcoplanarity'],histtype='step',bins=28,range=(0,np.pi*2))

#add axis labels to this plot
plt.xlabel('$\\varphi^*_{CP}$')
plt.ylabel('Weights (arbitrary units)')
#add legend to this plot
plt.legend(legend_labels)
plt.savefig(f'SignedAcoplanarity_Distribution.png')  # Save the plot
plt.show()



histograms = {}
# Create a ROOT file to save the histograms
output_file = TFile(f'H_tautau_ONN.root', 'RECREATE')

for phi, X_phi_scaled in X_scaled.items():
    hist_name = f'hist_ONN_phi_{phi}'
    hist_title = f'ONN ($\\phi$ = {phi}°)'

    histograms[phi] = TH1D(hist_name, hist_title, 28, -1, 1)



# Fill the histograms with the data from the DataFrame
for phi, X_phi_scaled in X_scaled.items():
    for value in classifier.predict_proba(X_phi_scaled)[:,0]-classifier.predict_proba(X_phi_scaled)[:,1]:
        histograms[phi].Fill(value)

    # write the histogram to the ROOT file
    histograms[phi].Write()

# Close the ROOT file
output_file.Close()