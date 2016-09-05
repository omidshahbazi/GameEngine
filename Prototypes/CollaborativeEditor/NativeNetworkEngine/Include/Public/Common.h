#ifndef COMMON_H
#define COMMON_H

#ifdef DECLARE_STATIC_LINK
#define NETWORK_ENGINE_API __declspec(dllimport)
#elif DECLARE_DYNAMIC_LINK
#define NETWORK_ENGINE_API __declspec(dllexport)
#else
#define NETWORK_ENGINE_API
#endif

#endif