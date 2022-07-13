#include "model_fmu_export.hpp"
#include "../model/include/model/model.h"
#include <cstddef>
#include <fmi2FunctionTypes.h>
#include <string>

using UnderlyingModel = LumpedMass;

struct ModelFMULayer : UnderlyingModel {
    // adds on extra variables and functions required to support FMU integration
    // double start_time = 0.0;

    ModelFMULayer(std::string const & id, float temperatureInit, float mass, float _cp) : UnderlyingModel{temperatureInit, mass, _cp} {};
    std::string fmu_id;
    void exchange(const bool force = false) {};
    void setStartTime(double time) noexcept {};
    void setTime(const double time) {};
    // Set value by index
    // Throws a std::exception if index is invalid or the simulation is not running
    void setValue(unsigned int index, double value) {};
    // Get the value by index
    // Throws a std::exception if index is invalid or the simulation is not running
    double getValue(unsigned int index) const {return 3.14;};
    void start() {};
    void stop() {};
    [[nodiscard]] double nextEventTime() const {return 3.14;};
    [[nodiscard]] bool operator==(const UnderlyingModel &other) const noexcept
    {
        return (this == &other);
    }
};

extern "C" {

MODEL_FMU_API fmi2Component fmi2Instantiate([[maybe_unused]] fmi2String instanceName,
                                        fmi2Type fmuType,
                                        fmi2String fmuGUID,
                                        fmi2String fmuResourceURI,
                                        const fmi2CallbackFunctions *functions,
                                        fmi2Boolean visible,
                                        fmi2Boolean loggingOn);

MODEL_FMU_API fmi2Status fmi2SetupExperiment(fmi2Component c,
                                         fmi2Boolean toleranceDefined,
                                         fmi2Real tolerance,
                                         fmi2Real startTime,
                                         fmi2Boolean stopTimeDefined,
                                         fmi2Real stopTime);

MODEL_FMU_API fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time);

MODEL_FMU_API fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real values[]);

MODEL_FMU_API fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real values[]);

MODEL_FMU_API fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo *fmi2eventInfo);

MODEL_FMU_API fmi2Status fmi2Terminate(fmi2Component c);

MODEL_FMU_API const char *fmi2GetTypesPlatform(void);

MODEL_FMU_API const char *fmi2GetVersion(void);

MODEL_FMU_API fmi2Status fmi2SetDebugLogging(fmi2Component, fmi2Boolean, size_t, const fmi2String[]);

MODEL_FMU_API fmi2Status fmi2Reset(fmi2Component);

MODEL_FMU_API void fmi2FreeInstance(fmi2Component);

MODEL_FMU_API fmi2Status fmi2EnterInitializationMode(fmi2Component);

MODEL_FMU_API fmi2Status fmi2ExitInitializationMode(fmi2Component);

MODEL_FMU_API fmi2Status fmi2GetInteger(fmi2Component, const fmi2ValueReference[], size_t, fmi2Integer[]);

MODEL_FMU_API fmi2Status fmi2GetBoolean(fmi2Component, const fmi2ValueReference[], size_t, fmi2Boolean[]);

MODEL_FMU_API fmi2Status fmi2GetString(fmi2Component, const fmi2ValueReference[], size_t, fmi2String[]);

MODEL_FMU_API fmi2Status fmi2SetInteger(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Integer[]);

MODEL_FMU_API fmi2Status fmi2SetBoolean(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Boolean[]);

MODEL_FMU_API fmi2Status fmi2SetString(fmi2Component, const fmi2ValueReference[], size_t, const fmi2String[]);

MODEL_FMU_API fmi2Status fmi2EnterEventMode(fmi2Component);

MODEL_FMU_API fmi2Status fmi2EnterContinuousTimeMode(fmi2Component);

MODEL_FMU_API fmi2Status fmi2CompletedIntegratorStep(fmi2Component, fmi2Boolean, fmi2Boolean *, fmi2Boolean *);

MODEL_FMU_API fmi2Status fmi2SetContinuousStates(fmi2Component, const fmi2Real[], size_t);

MODEL_FMU_API fmi2Status fmi2GetDerivatives(fmi2Component, fmi2Real[], size_t);

MODEL_FMU_API fmi2Status fmi2GetEventIndicators(fmi2Component, fmi2Real[], size_t);

MODEL_FMU_API fmi2Status fmi2GetContinuousStates(fmi2Component, fmi2Real[], size_t);

MODEL_FMU_API fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component, fmi2Real[], size_t);

}