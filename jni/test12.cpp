#include <android/log.h>
#include <stdio.h>
#include <substrate.h>

#include <dlfcn.h>

#define TAG "HOOKTEST"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

//MSConfig(MSFilterLibrary, "libnative-lib.so");
MSConfig(MSFilterExecutable, "/system/bin/app_process");


char* (*getFromNativeString) (void);

char* newhello(void)
{	static char str[] = "hello better tomorrow";
//    return "hello better tomorrow from cqh";
	return str;
}

void * lookup_symbol(char* libraryname,char* symbolname)
{
    void *handle = dlopen(libraryname,RTLD_GLOBAL | RTLD_NOW);

    if(handle != NULL){
        void *symbol = dlsym(handle,symbolname);

        if(symbol != NULL){
        	LOGD("cannot find smbol: %s\n", symbolname);
            return symbol;
        } else{
        	LOGD("dl error: %s",dlerror());
            return NULL;
        }
    } else{
    	LOGD("cannot find %s\n", libraryname);
        return NULL;
    }
}


//Substrate entry point
MSInitialize{
    LOGD("Substrate initialized.");
//    LOGD();
    MSImageRef image;
    char soPath[] = "/data/app-lib/zcgames.com.cynativeexample-1/libnative-lib.so";
    char funcName[] = "createHello";
	void* dvmload = lookup_symbol(soPath, funcName);
	if (dvmload !=NULL){
			MSHookFunction(dvmload,(void*)&newhello,(void **)&getFromNativeString);
	} else {
		LOGD("can not find libnative-lib.");
	}



}
