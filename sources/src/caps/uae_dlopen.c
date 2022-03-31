#include "sysconfig.h"
#include "sysdeps.h"
#include "uae/api.h"
#include "uae/dlopen.h"

#ifdef _WIN32
#include "windows.h"
#define UAE_DLOPEN_SUPPORT 1
#else
#if defined(HAVE_DLOPEN)
#include <dlfcn.h>
#define UAE_DLOPEN_SUPPORT 1
#endif
#endif

#ifdef UAE_DLOPEN_SUPPORT
UAE_DLHANDLE uae_dlopen(const TCHAR *path)
{
	UAE_DLHANDLE result;
	if (path == NULL || path[0] == _T('\0')) {
		write_log(_T("DLOPEN: No path given\n"));
		return NULL;
	}
#ifdef _WIN32
	result = LoadLibrary(path);
#else
	result = dlopen(path, RTLD_NOW);
	const char *error = dlerror();
	if (error != NULL)  {
		write_log("DLOPEN: %s\n", error);
	}
#endif
	if (result == NULL) {
		write_log("DLOPEN: Failed to open %s\n", path);
	}
	return result;
}

void *uae_dlsym(UAE_DLHANDLE handle, const char *name)
{
#if 0
	if (handle == NULL) {
		return NULL;
	}
#endif
#ifdef _WIN32
	return (void *) GetProcAddress(handle, name);
#else
	return dlsym(handle, name);
#endif
}

void uae_dlclose(UAE_DLHANDLE handle)
{
#ifdef _WIN32
	FreeLibrary (handle);
#else
	dlclose(handle);
#endif
}

// DLOPEN not supported
#else
UAE_DLHANDLE uae_dlopen(const TCHAR *path)
{
	write_log(_T("DLOPEN not supported\n"));
	return NULL;
}

void *uae_dlsym(UAE_DLHANDLE handle, const char *name)
{
	write_log(_T("DLSYM not supported\n"));
	return NULL;
}

void uae_dlclose(UAE_DLHANDLE handle)
{
	return;
}
#endif
