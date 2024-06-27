"""SPS calibration and evaluation

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
####################### NEGATIVE VOLTAGE #######################
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
datafiles = ["data/calibration_data/sps1_voltage_n3.3ton0.1v.csv"] # load voltage

#%%
### Load into a data frame ##
with open(cfg_path, "r") as f:
    cfg = yaml.load(f, Loader=Loader)

data = load_data(cfg, datafiles)

#%%
### Filter the 1st reading ###
indexes_to_drop = []
for i in range(0, len(data), 10):
    # Append the index to the list
    indexes_to_drop.append(i)

# Drop the rows with the specified indexes

data = data.drop(axis = 0, index=indexes_to_drop)


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

# plt.figure()
# plt.plot(data["V_in"], label = "SMU Voltage")
# plt.title("SMU")
# plt.xlabel("Index")
# plt.ylabel("(V)")
# plt.legend()
# plt.show()

# plt.figure()
# plt.plot(data["V_sps"], label = "Soil Power Sensor Raw")
# plt.title("Raw SPS")
# plt.xlabel("Index")
# plt.ylabel("SPS raw values")
# plt.legend()
# plt.show()

#%%
### Fit the linear model ###
v_input_cols = ["V_meas"]
v_model_neg = linear_model.LinearRegression()
v_model_neg.fit(data[v_input_cols], data["V_in"])

print("Voltage coefficients: ", v_model_neg.coef_, "Voltage intercept: ", v_model_neg.intercept_)

#%%
### Load the eval files ###
evalfiles = ["data/eval_data/sps1_voltage_eval_n3.3to0v.csv"]
eval_data = load_data(cfg, evalfiles)

#%%
### Test the fit ###
predicted = v_model_neg.predict(eval_data["V_meas"].values.reshape(-1, 1))

residuals = eval_data["V_in"] - predicted # Calculate the residuals

print("Evaluate using sklearn.metrics")
mae = mean_absolute_error(eval_data["V_in"], predicted)
rmse = np.sqrt(mean_squared_error(eval_data["V_in"], predicted))
r2 = r2_score(eval_data["V_in"], predicted)
mape = mean_absolute_percentage_error(eval_data["V_in"], predicted)
print(f"Mean absolute error: {mae:.4f}")
print(f"Mean absolute percentage error: {mape:.4f}")
print(f"Root mean square error: {rmse:.4f}")
print(f"R-squared: {r2:.4f}")

plt.figure()
plt.title("SPS to predicted voltage")
plt.scatter(eval_data["V_sps"], predicted, label = "Linear model")
plt.ylabel("Predicted (V)")
plt.xlabel("Raw SPS value")
plt.legend()
plt.show()

plt.figure()
plt.title("Residual plot")
plt.scatter(predicted, residuals)
plt.axhline(y=0, color='r', linestyle='--')
plt.ylabel("Residuals")
plt.xlabel("Predicted (V)")
plt.legend()
plt.show()

#%%
####################### POSITIVE VOLTAGE #######################
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
datafiles = ["data/calibration_data/sps1_voltage_0to3.3v.csv"] # load voltage

#%%
### Load into a data frame ##
with open(cfg_path, "r") as f:
    cfg = yaml.load(f, Loader=Loader)

data = load_data(cfg, datafiles)

#%%
### Filter the 1st reading ###
indexes_to_drop = []
for i in range(0, len(data), 10):
    # Append the index to the list
    indexes_to_drop.append(i)

# Drop the rows with the specified indexes

data = data.drop(axis = 0, index=indexes_to_drop)


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

# plt.figure()
# plt.plot(data["V_in"], label = "SMU Voltage")
# plt.title("SMU")
# plt.xlabel("Index")
# plt.ylabel("(V)")
# plt.legend()
# plt.show()

# plt.figure()
# plt.plot(data["V_sps"], label = "Soil Power Sensor Raw")
# plt.title("Raw SPS")
# plt.xlabel("Index")
# plt.ylabel("SPS raw values")
# plt.legend()
# plt.show()

#%%
### Fit the linear model ###
v_input_cols = ["V_meas"]
v_model_pos = linear_model.LinearRegression()
v_model_pos.fit(data[v_input_cols], data["V_in"])

print("Voltage coefficients: ", v_model_pos.coef_, "Voltage intercept: ", v_model_pos.intercept_)

#%%
### Load the eval files ###
evalfiles = ["data/eval_data/sps1_voltage_eval_0to2.2v.csv"]
eval_data = load_data(cfg, evalfiles)

#%%
### Test the fit ###
predicted = v_model_pos.predict(eval_data["V_meas"].values.reshape(-1, 1))

residuals = eval_data["V_in"] - predicted # Calculate the residuals

print("Evaluate using sklearn.metrics")
mae = mean_absolute_error(eval_data["V_in"], predicted)
rmse = np.sqrt(mean_squared_error(eval_data["V_in"], predicted))
r2 = r2_score(eval_data["V_in"], predicted)
mape = mean_absolute_percentage_error(eval_data["V_in"], predicted)
print(f"Mean absolute error: {mae:.4f}")
print(f"Mean absolute percentage error: {mape:.4f}")
print(f"Root mean square error: {rmse:.4f}")
print(f"R-squared: {r2:.4f}")


plt.figure()
plt.title("SPS to predicted voltage")
plt.scatter(eval_data["V_sps"], predicted, label = "Linear model")
plt.ylabel("Predicted (V)")
plt.xlabel("Raw SPS value")
plt.legend()
plt.show()

plt.figure()
plt.title("Residual plot")
plt.scatter(predicted, residuals)
plt.axhline(y=0, color='r', linestyle='--')
plt.ylabel("Residuals")
plt.xlabel("Predicted (V)")
plt.legend()
plt.show()

