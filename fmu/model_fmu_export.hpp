#ifndef MODEL_FMU_EXPORT_HPP
#define MODEL_FMU_EXPORT_HPP

#if (_WIN32 || _MSC_VER)
#ifdef model_fmu_EXPORTS
#define MODEL_FMU_API __declspec(dllexport)
#else
#define MODEL_FMU_API __declspec(dllimport)
#endif
#else
#define MODEL_FMU_API
#endif

#endif