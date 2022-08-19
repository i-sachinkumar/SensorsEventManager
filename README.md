  [![platform](https://img.shields.io/badge/platform-Android-yellow.svg)](https://www.android.com)
  [![API](https://img.shields.io/badge/API-16%2B-brightgreen.svg?style=plastic)](https://android-arsenal.com/api?level=16)
  [![License](https://img.shields.io/badge/license-MIT-4EB1BA.svg?style=flat-square)](https://www.apache.org/licenses/LICENSE-2.0.html)
  [![Android Arsenal]( https://img.shields.io/badge/Android%20Arsenal-SensorsEventManager-green.svg?style=flat )]( https://android-arsenal.com/details/1/6357 )
  <!--![Maven Central](https://img.shields.io/maven-central/v/io.github.DevComm-in/Toaster) -->
  

<!-- <a href="https://www.linkedin.com/in/"> -->
<!--    <img src="https://img.shields.io/badge/Support-Recommed%2FEndorse%20me%20on%20Linkedin-yellow?style=for-the-badge&logo=linkedin" alt="Connect with us" /></a> -->


# SensorsEventManager (Sensors Handeling Library)
- Android NDK library in C++, providing device’s motion sensors data<br>
- Header only ibrary<br>
- Super easy to Implement, handle, and extend<br>


### Header Files: 
Get it from <a href="https://github.com/i-sachinkumar/SensorsEventManager/tree/main/lib"> here</a>



## Example Use Case:
```c++
AccelerationEventQueue accelerationEventQueue;	//ACCELEROMETER
GyroscopeEventQueue gyroscopeEventQueue;	//GYROSCOPE


void enableAll() {
	accelerationEventQueue.enableSensor(ASENSOR_TYPE_ACCELEROMETER);
	gyroscopeEventQueue.enableSensor(ASENSOR_TYPE_GYROSCOPE);
}

void disableAll(){
	accelerationEventQueue.disableSensor();
	gyroscopeEventQueue.disableSensor();
}

bool isRecording = false;

while (isRecording) {
       //LOG("data given below")
       accelerationEventQueue.val[0]
       accelerationEventQueue.val[1]
       accelerationEventQueue.val[2] 
       gyroscopeEventQueue.val[0]
       gyroscopeEventQueue.val[1]
       gyroscopeEventQueue.val[2]
}

```

## Contributing<br>
Contributions are always welcome!
<br>See `contributing.md` for ways to get started.

Please adhere to this project's `code of conduct`.
