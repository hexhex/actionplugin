/* hand-made configuration, similar to config.h */

#define WIN32 

#if WIN32
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif
