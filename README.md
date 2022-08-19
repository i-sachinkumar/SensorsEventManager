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
Get it from <a href=""> here</a>



2. Add the dependency in app level `build.gradle`:
```bash
  dependencies {
	  implementation 'com.github.DevComm-in:Asyncer:v1.0.0'
  }
```



## Example Use Case:
```kotlin
class MainActivity : AppCompatActivity() {

    // start btn
    private lateinit var btn : Button
    
    //progressbar
    private lateinit var progressBar: ProgressBar

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btn = findViewById(R.id.btn)
        progressBar = findViewById(R.id.progress_bar)


        btn.setOnClickListener{
            onClick()
        }

    }


    //when start btn is clicked
    private fun onClick(){
    
        //showing indicator that background task is started
        progressBar.visibility = View.VISIBLE
        
        Asyncer().init(object : Task {
        
            //your background task
            override fun backgroundTask() {
                for(i in 1..100000){
                    Log.d("count", "backgroundTask: $i")
                }
            }

            /** what do you wanna do when task is cmpleted
            *   I am turning off progressBar and toasting a message 
            */
            override fun onTaskCompletion() {
                progressBar.visibility = View.GONE
                Toast.makeText(this@MainActivity, "Done", Toast.LENGTH_SHORT).show()
                Log.d("TAG", "onTaskCompletion")
            }
        })
    }

}

```

## Contributing<br>
Contributions are always welcome!
<br>See `contributing.md` for ways to get started.

Please adhere to this project's `code of conduct`.
