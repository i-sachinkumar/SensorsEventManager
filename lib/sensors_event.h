//
// Created by rjskg on 11-08-2022.
//

#include "scoped_thread.h"
#include <android/sensor.h>
#include <android/log.h>
#include <dlfcn.h>

#ifndef SENSOR_EVENT_MANAGEGER_SENSORS_EVENT_H
#define SENSOR_EVENT_MANAGEGER_SENSORS_EVENT_H

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     "TAG", __VA_ARGS__)


const char* kPackageName = "com.ihrsachin.sensoreventmanageger";

ASensorManager* AcquireASensorManagerInstance(void) {
    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *name);
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc = (PF_GETINSTANCEFORPACKAGE)
            dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
    if (getInstanceForPackageFunc) {
        return getInstanceForPackageFunc(kPackageName);
    }

    typedef ASensorManager *(*PF_GETINSTANCE)();
    PF_GETINSTANCE getInstanceFunc = (PF_GETINSTANCE)
            dlsym(androidHandle, "ASensorManager_getInstance");
    // by all means at this point, ASensorManager_getInstance should be available
    assert(getInstanceFunc);
    return getInstanceFunc();
}

ASensorManager *aSensorManager;

class SensorEventQueue{
protected:
    ScopedThread eventQueueThread;
    bool isEnable;
    const int kNumEvents = 1;
    const int kTimeoutMilliSecs = 10;
    const int kLooperId = 3;
    ASensorEventQueue *eventQueue;

    /**
     *
     * @param sensor_type
     * return int
     */
    void createEventQueue(int sensor_type){
        LOGI("Point 1");
        ALooper *looper;
        const ASensor *aSensor;   //TYPE_ANY
        aSensorManager = AcquireASensorManagerInstance();
        if (!aSensorManager) {
            return;
            LOGI("Failed to get a sensor manager");
        }

        looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

        eventQueue = ASensorManager_createEventQueue(
                aSensorManager,
                looper,
                kLooperId, nullptr
                /* no callback */, nullptr /* no data */);

        if (!eventQueue) {
            LOGI("Failed to create a sensor event queue");
            return;
        } else{
            LOGI("Created a sensor event queue");
        }

        aSensor = ASensorManager_getDefaultSensor(aSensorManager, sensor_type);
        if(aSensor == nullptr){
            LOGI("specified sensor is not found");
            return;
        }
        else
            LOGI("looper created");

        auto status = ASensorEventQueue_enableSensor(eventQueue, aSensor);
        if(status >=0) LOGI("Sensor Enabled");
        else
            LOGI("Failed to enable sensor");

        ASensorEventQueue_setEventRate(eventQueue, aSensor, 10000);

        while (isEnable) {
            int ident = ALooper_pollOnce(kTimeoutMilliSecs,
                                         nullptr /* no output file descriptor */,
                                         nullptr /* no output event */,
                                         nullptr /* no output data */);
            if (ident == kLooperId) {
//            LOGI("ident == kLooperId");
                ASensorEvent pEvent;
                if (ASensorEventQueue_getEvents(eventQueue, &pEvent,kNumEvents)) {
                    updateData(sensor_type, &pEvent);
                } else {
                    LOGI("Failed to get sensors' data");
                }
            }
        }

    }
    virtual void updateData(int sensor_type, ASensorEvent *pEvent) = 0;

public:
    /**
     *
     * @param sensor_type
     * @return
     */
    bool enableSensor(int sensor_type){
        isEnable = true;
        eventQueueThread.post(&SensorEventQueue::createEventQueue, this, sensor_type);
        return true;
    };
    void disableSensor(){
        isEnable = false;
        eventQueueThread.join();
    }
};


class AccelerationEventQueue : public SensorEventQueue{
    virtual void updateData(int sensor_type, ASensorEvent *pEvent) {
        val[0] = pEvent->acceleration.x;
        val[1] = pEvent->acceleration.y;
        val[2] = pEvent->acceleration.z;
        LOGI("acc x: %f, y: %f, z: %f", val[0], val[1], val[2]);
    }
public:
    float val[3];
};



class GyroscopeEventQueue : public SensorEventQueue{
    /**
     * check latency
     * @param sensor_type
     * @param pEvent
     */
    virtual void updateData(int sensor_type, ASensorEvent *pEvent) {
        val[0] = pEvent->uncalibrated_gyro.x_uncalib;
        val[1] = pEvent->uncalibrated_gyro.y_uncalib;
        val[2] = pEvent->uncalibrated_gyro.z_uncalib;
        val[3] = pEvent->uncalibrated_gyro.x_bias;
        val[4] = pEvent->uncalibrated_gyro.y_bias;
        val[5] = pEvent->uncalibrated_gyro.z_bias;
        LOGI("gyro x: %f, y: %f, z: %f", val[0], val[1], val[2]);
    }
public:
    float val[6];
};


class MagneticEventQueue : public SensorEventQueue{
    virtual void updateData(int sensor_type, ASensorEvent *pEvent) {
        val[0] = pEvent->magnetic.x;
        val[1] = pEvent->magnetic.y;
        val[2] = pEvent->magnetic.z;
        LOGI("mag x: %f, y: %f, z: %f", val[0], val[1], val[2]);
    }
public:
    float val[3];
};


class VectorEventQueue : public SensorEventQueue{
    virtual void updateData(int sensor_type, ASensorEvent *pEvent) {
        val[0] = pEvent->vector.x;
        val[1] = pEvent->vector.y;
        val[2] = pEvent->vector.z;
        LOGI("vector x: %f, y: %f, z: %f", val[0], val[1], val[2]);
    }
public:
    float val[3];
};


class StepCounter : public SensorEventQueue {
    virtual void updateData(int sensor_type, ASensorEvent *pEvent) {
        val = pEvent->u64.step_counter;
        LOGI("step_counter x: %f", val);
    }

public:
    float val;
};


#endif //SENSOR_EVENT_MANAGEGER_SENSORS_EVENT_H
