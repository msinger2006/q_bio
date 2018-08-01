'''
example of machine learning code using skopt for hyperparameter optimization.

Mike Singer
August 2017
'''

import numpy as np

from sklearn.datasets import make_moons
from sklearn.metrics import confusion_matrix
from sklearn.model_selection import StratifiedKFold
from sklearn.model_selection import cross_val_score
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from skopt import gp_minimize


RANDOM_STATE = 0  # random state for reproducability

N_SAMPLES = 1000  # number of data samples
DATA_NOISE = 0.5  # noise level in data
TEST_SIZE = 0.2   # percent of test data in train/test split
K_FOLDS = 5       # number of folds in cross-validation
N_CALLS = 50      # max number of optimizer calls in hyperparam loop


# hold/increment random seed for cross-validation in hyperparam loop
class Incrementer:
    def __init__(self, i):
        self.i = i

    def increment(self):
        self.i += 1
        return self.i


# hyperparameter evaluation function
def evaluate(params):
    C, gamma = params
    skfold = StratifiedKFold(n_splits=K_FOLDS,
                             random_state=incrementer.increment())
    svc.set_params(C=C, gamma=gamma)
    scores = cross_val_score(svc, x_train, y_train, scoring='neg_log_loss',
                             cv=skfold)
    value = -np.mean(scores)
    return value


# set random state for repeatability
np.random.seed(RANDOM_STATE)

# load data
data = make_moons(n_samples=N_SAMPLES, noise=DATA_NOISE,
                  random_state=RANDOM_STATE)
x = data[0]  # data set
y = data[1]  # labels

# split data into train/test sets with stratification
x_train, x_test, y_train, y_test = train_test_split(x, y,
                                                    test_size=TEST_SIZE,
                                                    stratify=y,
                                                    random_state=RANDOM_STATE)

# normalize data using scaler
scaler = StandardScaler()
scaler.fit(x_train)
x_train = scaler.transform(x_train)
x_test = scaler.transform(x_test)

# construct support vector classifier object
svc = SVC(probability=True)

# create incrementer object
incrementer = Incrementer(RANDOM_STATE)

# hyperparameter loop
hyper_params = [(0.1, 10.),  # range for C
                (0.001, 10.),  # range for gamma
                ]
optimizer = gp_minimize(evaluate, hyper_params, n_calls=N_CALLS,
                        random_state=RANDOM_STATE)

# print final values
print('final values = ', optimizer.x[0], optimizer.x[1])

# fit final svc model (with final values of hyperparameters) using
# training data, i.e., svc.set_params(C=C, gamma=gamma, coef0=coef0)
# where the values of C, gamma, coef0 are the final values obtained
# from skopt
svc.set_params(C=optimizer.x[0], gamma=optimizer.x[1])
model = svc.fit(x_train, y_train)

# make predictions using training and testing data sets (note: predictions
# from training set are for reference only -- inappropriate to assess
# model generalization performance based on training data)
y_train_predict = svc.predict(x_train)
y_test_predict = svc.predict(x_test)

# compute confusion matrices and sensitivity/specificity
train_conf_matrix = confusion_matrix(y_train, y_train_predict)
train_sens = 100.*train_conf_matrix[1, 1] / \
    (train_conf_matrix[1, 0] + train_conf_matrix[1, 1])
train_spec = 100.*train_conf_matrix[0, 0] / \
    (train_conf_matrix[0, 0] + train_conf_matrix[0, 1])

test_conf_matrix = confusion_matrix(y_test, y_test_predict)
test_sens = 100.*test_conf_matrix[1, 1] / \
    (test_conf_matrix[1, 0] + test_conf_matrix[1, 1])
test_spec = 100.*test_conf_matrix[0, 0] / \
    (test_conf_matrix[0, 0] + test_conf_matrix[0, 1])

# print final results
print('training sens/spec = ', train_sens, train_spec)
print('testing sens/spec = ', test_sens, test_spec)
