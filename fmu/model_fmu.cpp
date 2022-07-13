#include "model_fmu.hpp"
#include <fmi2Functions.h>
#include <iostream>
#include <list>
#include <regex>

namespace Model_fmu {

std::list<ModelFMULayer> models;

struct Bad_Pointer : public std::runtime_error {
    Bad_Pointer() : std::runtime_error("Invalid fmi2Component") {}
};

// FMI standard identifies components by void *. This function
// is used to retrieve the model instance corresponding to the
// given pointer.
// Throw an exception if comp does not identify an instance in the
// model list
ModelFMULayer &get_model(void *comp) {
    const auto it = std::find_if(std::begin(models), std::end(models), [comp](const ModelFMULayer &s) { return comp == &s; });

    if (it == std::end(models)) {
        throw Bad_Pointer();
    }

    // if (auto ep = it->lastException()) {
    //  std::rethrow_exception(ep);
    //}

    return *it;
}

void remove_model(ModelFMULayer &comp) {
    const auto it = std::find(models.begin(), models.end(), comp);
    if (it != models.end()) {
        models.erase(it);
    } else {
        throw std::runtime_error("Error in \"remove_model\"");
    }
}

fmi2Status handle_fmi_exception(ModelFMULayer &comp) {
    try {
        // rethrow the last exception
        // This is the Lippincott pattern https://www.youtube.com/watch?v=-amJL3AyADI
        throw;
    } catch (const std::runtime_error &e) {
        //        comp.logMessage(EnergyPlus::Error::Fatal, e.what());
        //        comp.emptyLogMessageQueue();
    } catch (...) {
        std::clog << "Unknown Exception\n";
    }

    return fmi2Error;
}

template <typename Function> fmi2Status with_model(fmi2Component c, Function f) {
    try {
        // Will throw if c is invalid
        auto &comp = get_model(c);
        try {
            f(comp);
            return fmi2OK;
        } catch (...) {
            // comp is passed so that we can log back the error message
            return handle_fmi_exception(comp);
        }
    } catch (const Bad_Pointer &e) {
        std::clog << e.what() << "\n";
    } catch (...) {
        std::clog << "Unknown Exception\n";
    }
    return fmi2Error;
}

} // namespace Model_fmu

MODEL_FMU_API fmi2Component fmi2Instantiate([[maybe_unused]] fmi2String instanceName, [[maybe_unused]] fmi2Type fmuType,
                                            fmi2String fmuGUID, [[maybe_unused]] fmi2String fmuResourceURI,
                                            [[maybe_unused]] const fmi2CallbackFunctions *functions, [[maybe_unused]] fmi2Boolean visible,
                                            [[maybe_unused]] fmi2Boolean loggingOn) {
    try {
        Model_fmu::models.emplace_back(fmuGUID, 0.0, 1100, 300);
        auto &comp = Model_fmu::models.back();
        return &comp;
    } catch (const std::runtime_error &e) {
        std::clog << e.what() << "\n";
    } catch (...) {
        std::clog << "Unknown Exception during EnergyPlus fmi2Instantiate\n";
    }
    return nullptr;
}

MODEL_FMU_API fmi2Status fmi2SetupExperiment(fmi2Component c, [[maybe_unused]] fmi2Boolean toleranceDefined, [[maybe_unused]] fmi2Real toleranceValue,
                                             [[maybe_unused]] fmi2Real startTime, [[maybe_unused]] fmi2Boolean stopTimeDefined,
                                             [[maybe_unused]] fmi2Real stopTime) {
    auto action = [&](ModelFMULayer &comp) { comp.setStartTime(startTime); };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time) {
    auto action = [&](ModelFMULayer &comp) { comp.setTime(time); };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real values[]) {
    auto action = [&](ModelFMULayer &comp) {
        for (size_t i = 0; i < nvr; ++i) {
            auto valueRef = vr[i];  // NOLINT
            auto value = values[i]; // NOLINT
            comp.setValue(valueRef, value);
        }
    };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real values[]) {
    auto action = [&](ModelFMULayer &comp) {
        // Call to start will be a no op if the simulation is already running
        comp.start();
        comp.exchange();
        std::transform(vr, std::next(vr, static_cast<std::ptrdiff_t>(nvr)), values, [&](const auto valueRef) { return comp.getValue(valueRef); });
    };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo *eventInfo) {
    auto action = [&](ModelFMULayer &comp) {
        eventInfo->newDiscreteStatesNeeded = fmi2False;
        eventInfo->nextEventTime = comp.nextEventTime();
        eventInfo->nextEventTimeDefined = fmi2True;
        eventInfo->terminateSimulation = fmi2False;
    };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2Terminate(fmi2Component c) {
    auto action = [&](ModelFMULayer &comp) {
        comp.stop();
        Model_fmu::remove_model(comp);
    };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API const char *fmi2GetTypesPlatform(void) { return fmi2TypesPlatform; }

MODEL_FMU_API const char *fmi2GetVersion(void) { return fmi2Version; }

MODEL_FMU_API fmi2Status fmi2SetDebugLogging(fmi2Component, fmi2Boolean, size_t, const fmi2String[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2Reset(fmi2Component) { return fmi2OK; }

MODEL_FMU_API void fmi2FreeInstance(fmi2Component c) {
    auto action = [&](ModelFMULayer &comp) { Model_fmu::remove_model(comp); };

    Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2EnterInitializationMode([[maybe_unused]] fmi2Component c) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2ExitInitializationMode(fmi2Component c) {
    auto action = [&](ModelFMULayer &comp) { comp.start(); };

    return Model_fmu::with_model(c, action);
}

MODEL_FMU_API fmi2Status fmi2GetInteger(fmi2Component, const fmi2ValueReference[], size_t, fmi2Integer[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2GetBoolean(fmi2Component, const fmi2ValueReference[], size_t, fmi2Boolean[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2GetString(fmi2Component, const fmi2ValueReference[], size_t, fmi2String[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2SetInteger(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Integer[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2SetBoolean(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Boolean[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2SetString(fmi2Component, const fmi2ValueReference[], size_t, const fmi2String[]) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2EnterEventMode(fmi2Component) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2EnterContinuousTimeMode(fmi2Component) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2CompletedIntegratorStep(fmi2Component, fmi2Boolean, fmi2Boolean *enterEventMode, fmi2Boolean *terminateSimulation) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2SetContinuousStates(fmi2Component, const fmi2Real[], size_t) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2GetDerivatives(fmi2Component, fmi2Real[], size_t) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2GetEventIndicators(fmi2Component, fmi2Real[], size_t) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2GetContinuousStates(fmi2Component, fmi2Real[], size_t) { return fmi2OK; }

MODEL_FMU_API fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component, fmi2Real[], size_t) { return fmi2OK; }