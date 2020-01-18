#ifndef COMMON_H
#define COMMON_H

#ifdef DECLARE_STATIC_LINK
#define CORE_ENGINE_API __declspec(dllimport)
#elif DECLARE_DYNAMIC_LINK
#define CORE_ENGINE_API __declspec(dllexport)
#else
#define CORE_ENGINE_API
#endif

#endif