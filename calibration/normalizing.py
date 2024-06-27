"""SPS normalizing to remove non-linearity

The adc data might be non-linear, so this file will attempt to remove said non-linearity from the ADC readings

Stephen Taylor 5/20/2024
"""
#%%
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from rocketlogger.data import RocketLoggerData
from sklearn import linear_model
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score, mean_absolute_percentage_error
import yaml
import pdb
try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader

#%%
### Load the data ###
def load_data(cfg, datafiles):
    df_list = []
    for d in datafiles:
        df = pd.read_csv(d)
        df_list.append(df)
    
    data = pd.concat(df_list, ignore_index=True)
    #data = data.set_index("V")

    data["V_in"] = data["V_in"]
    data["I_in"] = data["I_in"] 
    data["I_meas"] = data["I_sps"] 
    data["V_meas"] = data["V_sps"]
    
    return data

#%%
### Load the calibration CSVs ###
cfg_path = "data/config.yaml"
datafiles_pos = ["data/calibration_data/sps1_voltage_0to3.3v.csv"] # load voltage positive
datafiles_neg = ["data/calibration_data/sps1_voltage_n3.3ton0.1v.csv"] # load voltage negative

#%%
### Load into a data frame ##
with open(cfg_path, "r") as f:
    cfg = yaml.load(f, Loader=Loader)

data = load_data(cfg, datafiles_pos + datafiles_neg)

#%%
### Filter the 1st reading ###
indexes_to_drop = []
for i in range(0, len(data), 10):
    # Append the index to the list
    indexes_to_drop.append(i)


# Drop the rows with the specified indexes
data = data.drop(axis = 0, index=indexes_to_drop)

#%%
### Fit the linear model ###
v_input_cols = ["V_meas"]
v_model = linear_model.LinearRegression()
v_model.fit(data[v_input_cols], data["V_in"])

print("Voltage coefficients: ", v_model.coef_, "Voltage intercept: ", v_model.intercept_)

#%%
### Load the eval files ###
evalfiles_pos = ["data/eval_data/sps1_voltage_eval_0to2.2v.csv"]
evalfiles_neg = ["data/eval_data/sps1_voltage_eval_n3.3to0v.csv"]
eval_data = load_data(cfg, evalfiles_pos + evalfiles_neg)

### Filter the 1st reading ###
indexes_to_drop = []
for i in range(0, len(eval_data), 10):
    # Append the index to the list
    indexes_to_drop.append(i)

#%%
#### Plot the SMU voltage and the raw SPS values to check for linearity ###
plt.figure()
#for i, _ in enumerate(data["V_in"].to_list()[10:20]):
    #plt.scatter(data["V_in"].to_list()[i], data["V_sps"].to_list()[i], s=3, label = f"{i}")
plt.scatter(data["V_in"], data["V_sps"], s=3)
plt.title("Data visualization")
plt.xlabel("Input (V)")
plt.ylabel("ADC Raw")
plt.legend()
plt.show()

#%%
### Test the fit ###
predicted = v_model.predict(eval_data["V_meas"].values.reshape(-1, 1))

residuals = eval_data["V_in"] - predicted # Calculate the residuals

plt.figure()
plt.title("Residual plot")
plt.scatter(predicted, residuals)
plt.axhline(y=0, color='r', linestyle='--')
plt.ylabel("Residuals")
plt.xlabel("Predicted (V)")
plt.legend()
plt.show()
# %%
