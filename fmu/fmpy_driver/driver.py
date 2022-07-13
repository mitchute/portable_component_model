import numpy as np
import matplotlib.pyplot as plt
from fmpy import *

fmu = 'CrankCase_ME2.fmu'
dump(fmu)

# read the model description
model_description = read_model_description(fmu)

# collect the value references
vrs = {}
for variable in model_description.modelVariables:
    vrs[variable.name] = variable.valueReference

L = 412
hPLug = 215
alphaDrvie = 60
start_vrs = [vrs['l1'], vrs['h_plug'], vrs['Alpha_drive']]
start_values = [L, hPLug, alphaDrvie]

dtype = [('time', np.double), ('l1', np.int), ('h_plug', np.int), ('Alpha_drive', np.int)]
signals = np.array([(0.0, L, hPLug, alphaDrvie)], dtype=dtype)

print_interval = 1e-5
result = simulate_fmu(fmu, start_time=0, stop_time=0.1, relative_tolerance=1e-7, output_interval=print_interval, input=signals)

t = result['time']
y_Force = result['expseu_.Out2'] # y force
plt.plot(t, y_Force)