## 使用Eclipse
### `File`-> `New`-> `Other`->`Android`-> `Android Application Project`, 
applicationName为`test11`
Minimum Required SDK: `API 14:***`

### 右键 `test11`-> `Android tools` -> `Add Native supports`
输入`test11`

### 复制`substrate.h`, `libsubstrate.so`, `libsubstrate-dvm.so`到`test11/jni`这个目录里面

### 在`test11/jni` 修改`Android.mk`, 输入
```
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := substrate
LOCAL_SRC_FILES := libsubstrate.so
include $(PREBUILT_SHARED_LIBRARY)
 
include $(CLEAR_VARS)
LOCAL_MODULE    := substrate-dvm
LOCAL_SRC_FILES := libsubstrate-dvm.so
include $(PREBUILT_SHARED_LIBRARY)
 
include $(CLEAR_VARS)
# 注意：此处substrate规定模块名必须以.cy结尾
LOCAL_MODULE    := test11.cy 
LOCAL_SRC_FILES := test11.cpp
LOCAL_LDLIBS := -llog -ldl
LOCAL_ARM_MODE := arm
LOCAL_LDLIBS += -L$(LOCAL_PATH) -lsubstrate-dvm -lsubstrate
include $(BUILD_SHARED_LIBRARY)
```

### 修改test11.cpp
```
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
//不使用MSGetImageByName,因为MSGetImageByName可能无法找到
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

```
注意事项:
soPath这个是通过`cat /proc/{进程ID}/maps | grep 'so'`, 找到的， 并不是app安装目录的so
### 修改Manifest.xml
application添加属性`android:hasCode="false"`, manifest添加下级`<uses-permission android:name="cydia.permission.SUBSTRATE"/>`
最终结果为:
```
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.test11"
    android:versionCode="1"
    android:versionName="1.0" >

    <uses-sdk
        android:minSdkVersion="14"
        android:targetSdkVersion="21" />

    <application
        android:hasCode="false"
        android:allowBackup="true"
        android:icon="@drawable/ic_launcher"
        android:label="@string/app_name"
        android:theme="@style/AppTheme" >
        <activity
            android:name=".MainActivity"
            android:label="@string/app_name" >
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />

                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
	<uses-permission android:name="cydia.permission.SUBSTRATE"/>
</manifest>

```