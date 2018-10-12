## ʹ��Eclipse
### `File`-> `New`-> `Other`->`Android`-> `Android Application Project`, 
applicationNameΪ`test11`
Minimum Required SDK: `API 14:***`

### �Ҽ� `test11`-> `Android tools` -> `Add Native supports`
����`test11`

### ����`substrate.h`, `libsubstrate.so`, `libsubstrate-dvm.so`��`test11/jni`���Ŀ¼����

### ��`test11/jni` �޸�`Android.mk`, ����
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
# ע�⣺�˴�substrate�涨ģ����������.cy��β
LOCAL_MODULE    := test11.cy 
LOCAL_SRC_FILES := test11.cpp
LOCAL_LDLIBS := -llog -ldl
LOCAL_ARM_MODE := arm
LOCAL_LDLIBS += -L$(LOCAL_PATH) -lsubstrate-dvm -lsubstrate
include $(BUILD_SHARED_LIBRARY)
```

### �޸�test11.cpp
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
//��ʹ��MSGetImageByName,��ΪMSGetImageByName�����޷��ҵ�
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
ע������:
soPath�����ͨ��`cat /proc/{����ID}/maps | grep 'so'`, �ҵ��ģ� ������app��װĿ¼��so
### �޸�Manifest.xml
application�������`android:hasCode="false"`, manifest����¼�`<uses-permission android:name="cydia.permission.SUBSTRATE"/>`
���ս��Ϊ:
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