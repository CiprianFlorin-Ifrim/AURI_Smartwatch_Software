//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------LIBRARIES----------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//system libraries
#include <Wire.h>                                                                                                                                                                     // I2C Protocol library (Nicla Sense ME custom)
#include <PDM.h>                                                                                                                                                                      // microphone recording library
#include <I2C_Anything.h>                                                                                                                                                             // library to send any data type through I2C (normally low/high bits) 

//python trained machine learning models ported to plain C for arduino integration
#include "src/ml-models/SVMClassifier.h"
#include "src/ml-models/XGBoostClassifier.h"
#include "src/ml-models/DecisionTreeClassifier.h"
#include "src/ml-models/GaussianNaiveBayesClassifier.h"
#include "src/ml-models/DecisionTreeRegressor.h"

//trained deep learning model library for voice commands recognition
#include <dl_cnn_voice_inferencing.h>



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------DECLARATIONS--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define EIDSP_QUANTIZE_FILTERBANK   0                                                                                                                                                 // if the target is limited in memory remove this macro to save 10K RAM
#define slaveAddress 0x08                                                                                                                                                             // I2C address

//machine learning models declarations
Eloquent::ML::Port::SVM SVM_classifier;                                                                                                                                               // define SVM Classification object
Eloquent::ML::Port::XGBClassifier XGBoost_classifier;                                                                                                                                 // define XGBoost Classifier object
Eloquent::ML::Port::DecisionTree DecisionTree_classifier;                                                                                                                             // define Decision Tree Classifier object
Eloquent::ML::Port::GaussianNB GaussianNB_classifier;                                                                                                                                 // define Gaussian Naive Bayes Classifier object
Eloquent::ML::Port::DecisionTreeRegressor decisiontree_regressor;                                                                                                                     // define Decision Tree Regressor object

//-----------------------------------------------------------------------------------------------------------------------------------DEEP LEARNING VOICE AUDIO BUFFERS & MEMORY POINTERS----------------------------------------------------------------------------------------------------------------------------------------------
typedef struct {                                                                                                                                                                      // buffer structure
  int16_t  *buffer;
  uint8_t  buf_ready;
  uint32_t buf_count;
  uint32_t n_samples;
} inference_t;

static inference_t inference;                                                                                                                                                         // inference variable 
static signed short sampleBuffer[2048];                                                                                                                                               // audio sample buffer
static bool debug_nn = false;                                                                                                                                                         // set this to true to see e.g. features generated from the raw signal

//-------------------------------------------------------------------------------------------------------------------------------------------------------VARIABLES----------------------------------------------------------------------------------------------------------------------------------------------------------
float Y[6];                                                                                                                                                                           // labels output array 
float X[5];                                                                                                                                                                           // 5 features array for XGBoost, Decision Tree, Gaussian Naive Bayes & Decision Tree Regressor
float X_SVM[6];                                                                                                                                                                       // 6 features array specifically for Support Vector Machines weather classification model
bool startInferencing = false;                                                                                                                                                        // recording & CNN classification start flag



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------SYSTEM SETUP--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {                                                                                                                                                                        // Arduino setup function
  Wire.begin(slaveAddress);                                                                                                                                                           // join i2c bus with address #8
  Wire.onRequest(requestEvent);                                                                                                                                                       // register event
  Wire.onReceive(receiveEvent);                                                                                                                                                       // register event
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------MAIN ROUTINE--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  if (startInferencing == true) {                                                                                                                                                     // if the recording flag is true, then start the recording and classification
    Y[5] = 14;                                                                                                                                                                        // default label in case the classified recording has all labels lower than 50% (softmax function)

    if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {                                                                                                        // if the result is equal to false, then:
      Y[5] = 15;                                                                                                                                                                      // there have been issues with starting the audio sampling
      startInferencing = false;                                                                                                                                                       // set the flag to false
      return;                                                                                                                                                                         // return back to the main loop outside the if
    }
    delay(350);                                                                                                                                                                       // 350 milliseconds delay for the user to "react" (audio stimulus is 170ms + the microphone with the PDM library starts a bit early)  
                                                                                                                                                                                      
    bool m = microphone_inference_record();                                                                                                                                           // set the variable to the either false or true if the recording can start
    if (!m) {                                                                                                                                                                         // if the variable is true then proceed
      Y[5] = 16;                                                                                                                                                                      // set the voice label as 12 which mean "Software or Hardware issue detected. Failed to record audio!"
      startInferencing = false;                                                                                                                                                       // set the flag to false                                                             
      return;                                                                                                                                                                         // return back to the main loop outside the if
    }

    signal_t signal;                                                                                                                                                                  // setup classifier 
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;                                                                                                                             // set audio length
    signal.get_data = &microphone_audio_signal_get_data;                                                                                                                              // result array
    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);                                                                                                                  // start classification on the signal
    if (r != EI_IMPULSE_OK) {                                                                                                                                                         // if there has been an anomaly with the classifier
      Y[5] = 17;                                                                                                                                                                      // set the label as 13 meaning: "Deep Learning issue detected. Failed to run the classifier!"
      startInferencing = false;                                                                                                                                                       // set the flag to false                                                                                               
      return;                                                                                                                                                                         // return back to the main loop outside the if
    }

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {                                                                                                                       // loop through all the predictions
      if (result.classification[ix].value > 0.5) Y[5] = ix;                                                                                                                           // if there is a prediction with higher than 50% certainty (softmax function - everything adds to 100%)                                                                               
    }                                                                                                                                                                                 // set the predicted label to the label number from Edge Impulse CNN classifier

    Y[0] = SVM_classifier.predict(X_SVM);                                                                                                                                             // use the 6 features array with the SVM to predict the rainfall, and assign it to element 0 in the array
    Y[1] = XGBoost_classifier.predict(X);                                                                                                                                             // use the 5 features array with the XGB to predict the rainfall, and assign it to element 1 in the array
    Y[2] = DecisionTree_classifier.predict(X);                                                                                                                                        // use the 5 features array with the DTC to predict the rainfall, and assign it to element 2 in the array
    Y[3] = GaussianNB_classifier.predict(X);                                                                                                                                          // use the 5 features array with the GNB to predict the rainfall, and assign it to element 3 in the array
    Y[4] = decisiontree_regressor.predict(X);                                                                                                                                         // use the 5 features array with the DTR to predict the rainfall, and assign it to element 4 in the array

    microphone_inference_end();                                                                                                                                                       // end the voice inference
    startInferencing = false;                                                                                                                                                         // set the flag to false as the process has ended
  }
  delay(10);                                                                                                                                                                          // delay for system stability
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------SUBROUTINES---------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------I2C RECEIVE DATA EVENT INTERRUPT----------------------------------------------------------------------------------------------------------------------------------------------
void receiveEvent(int howMany) {
  for (int i = 0; i < 6; i++) {                                                                                                                                                       // loop through all the received data (6 elements)
    I2C_readAnything(X_SVM[i]);                                                                                                                                                       // place it in the array with 6 elements for the SVM ML Classifier (uses 6 features for classification)
    if (i < 5) X[i] = X_SVM[i];                                                                                                                                                       // if i is less than 5, take the first 5 elements and place them in the regular array for ML Classification 
  }                                                                                                                                                                                   // 4 out of 5 models use 4 features
  
  startInferencing = true;                                                                                                                                                            // change flag for voice inference to true, so that the main loop proceeds only after data has been received
}

//--------------------------------------------------------------------------------------------------------------------------------------------I2C REQUEST DATA EVENT INTERRUPT----------------------------------------------------------------------------------------------------------------------------------------------
void requestEvent(void) {
  for (int j = 0; j < sizeof(Y); j++) I2C_writeAnything(Y[j]);                                                                                                                        // loop through the labels array, then send labels one by one
}

//----------------------------------------------------------------------------------------------------------------------------------------------------SPEECH FUNCTION-------------------------------------------------------------------------------------------------------------------------------------------------------
static void pdm_data_ready_inference_callback(void) {                                                                                                                                 // PDM buffer full callback
  int bytesAvailable = PDM.available();                                                                                                                                               // get data and call audio thread callback
  int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);                                                                                                                 // read into the sample buffer

  if (inference.buf_ready == 0) {                                                                                                                                                       
    for (int i = 0; i < bytesRead >> 1; i++) {                                                                                                                                        // perform loop for bytesRead-1 iterations, shift value by 1 bit
      inference.buffer[inference.buf_count++] = sampleBuffer[i];                                                                                                                      // place the sample in the inference buffer

      if (inference.buf_count >= inference.n_samples) {                                                                                                                               // if the count is higher than the amount of samples available then:
        inference.buf_count = 0;                                                                                                                                                      // set the count inference buffer to 0  
        inference.buf_ready = 1;                                                                                                                                                      // the buffer is not ready
        break;                                                                                                                                                                        // exit the for loop early
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------START VOICE RECORDING---------------------------------------------------------------------------------------------------------------------------------------------------
static bool microphone_inference_start(uint32_t n_samples) {                                                                                                                          // init inferencing struct and setup/start PDM
  inference.buffer = (int16_t *)malloc(n_samples * sizeof(int16_t));                                                                                                                  // allocates a block of size bytes of memory, returning a pointer to the beginning of the block
  if (inference.buffer == NULL) return false;                                                                                                                                         // return false if the buffer is zero

  inference.buf_count  = 0;                                                                                                                                                           // set the count inference buffer to 0  
  inference.n_samples  = n_samples;                                                                                                                                                   // set the samples for the buffer equal to the n_samples (in this case 1)
  inference.buf_ready  = 0;                                                                                                                                                           // set the ready inference buffer to 0

  PDM.onReceive(&pdm_data_ready_inference_callback);                                                                                                                                  // configure the data receive callback
  PDM.setBufferSize(4096);                                                                                                                                                            // set buffer size
  if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {                                                                                                                                       // initialize PDM with: one channel (mono mode), a 16 kHz sample rate
    Y[5] = 16;                                                                                                                                                                        // set the voice label to send back to the Nicla Sense ME as 12 (meaning "issues with the audio sampling")
    microphone_inference_end();                                                                                                                                                       // end recording

    return false;                                                                                                                                                                     // return false because the microphone recording was unable to start
  }

  PDM.setGain(127);                                                                                                                                                                   // set the gain, defaults to 20
  return true;                                                                                                                                                                        // return true if the microphone recording was able to start
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------RECORDING BUFFER-----------------------------------------------------------------------------------------------------------------------------------------------------
static bool microphone_inference_record(void) {
  inference.buf_ready = 0;                                                                                                                                                            // set the ready inference buffer to 0
  inference.buf_count = 0;                                                                                                                                                            // set the count inference buffer to 0  

  while (inference.buf_ready == 0) delay(10);                                                                                                                                         // wait on new data
  return true;                                                                                                                                                                        // true when finished
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------AUDIO DATA--------------------------------------------------------------------------------------------------------------------------------------------------------
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr) {
  numpy::int16_to_float(&inference.buffer[offset], out_ptr, length);                                                                                                                  // get raw audio signal data
  return 0;                                                                                                                                                                           // return value
}

//----------------------------------------------------------------------------------------------------------------------------------------------------END VOICE RECORDING---------------------------------------------------------------------------------------------------------------------------------------------------
static void microphone_inference_end(void) {
  PDM.end();                                                                                                                                                                          // stop PDM and release buffers
  free(inference.buffer);                                                                                                                                                             // release voice buffer                                                                                                                                                                                                                 
}
