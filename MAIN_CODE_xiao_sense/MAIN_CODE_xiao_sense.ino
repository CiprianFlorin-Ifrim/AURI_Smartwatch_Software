#define EIDSP_QUANTIZE_FILTERBANK   0       // If the target is limited in memory remove this macro to save 10K RAM

/* Includes ---------------------------------------------------------------- */
#include <Wire.h>
#include <PDM.h>
#include <a1210_samples_cnn_voice_rec_inferencing.h>

/** Audio buffers, pointers and selectors */
typedef struct {
  int16_t *buffer;
  uint8_t buf_ready;
  uint32_t buf_count;
  uint32_t n_samples;
} inference_t;

static inference_t inference;
static signed short sampleBuffer[2048];
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

int label = 0;
/**
   @brief      Arduino setup function
*/
bool startInferencing = false;

void setup()
{

  Serial.begin(115200);    // start serial for output
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
}

void loop()
{
  if (startInferencing == true) {
    label = 10;
        
    if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {
      label = 11;
      startInferencing = false;
      return;
    }
    delay(350);

    bool m = microphone_inference_record();
    if (!m) {
      label = 12;
      startInferencing = false;
      return;
    }

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &microphone_audio_signal_get_data;
    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);
    if (r != EI_IMPULSE_OK) {
      label = 13;
      startInferencing = false;
      return;
    }

    
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {        //loop through all the predictions
      if (result.classification[ix].value > 0.5) {                     //if there is a prediction with higher than 50% certainty (softmax function - everything adds to 100%)
        label = ix;                                                    //set the predicted label to the label number from Edge Impulse CNN classifier
      }
    }

    microphone_inference_end();
    startInferencing = false;
  }
  delay(100);
}

void receiveEvent(int howMany) {
  startInferencing = true;
}

void requestEvent() {
  Wire.write(label);                                                       // respond with message of 1 byte
}

static void pdm_data_ready_inference_callback(void)                        //PDM buffer full callback
{
  int bytesAvailable = PDM.available();                                    //Get data and call audio thread callback
  int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);      // read into the sample buffer

  if (inference.buf_ready == 0) {
    for (int i = 0; i < bytesRead >> 1; i++) {
      inference.buffer[inference.buf_count++] = sampleBuffer[i];

      if (inference.buf_count >= inference.n_samples) {
        inference.buf_count = 0;
        inference.buf_ready = 1;
        break;
      }
    }
  }
}

static bool microphone_inference_start(uint32_t n_samples)                //Init inferencing struct and setup/start PDM
{
  inference.buffer = (int16_t *)malloc(n_samples * sizeof(int16_t));

  if (inference.buffer == NULL) {
    return false;
  }

  inference.buf_count  = 0;
  inference.n_samples  = n_samples;
  inference.buf_ready  = 0;

  PDM.onReceive(&pdm_data_ready_inference_callback);          // configure the data receive callback
  PDM.setBufferSize(4096);
  if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {                //initialize PDM with: one channel (mono mode), a 16 kHz sample rate
    label = 12;
    microphone_inference_end();

    return false;
  }

  
  PDM.setGain(127);                                        // set the gain, defaults to 20

  return true;
}

static bool microphone_inference_record(void)
{
  inference.buf_ready = 0;
  inference.buf_count = 0;

  while (inference.buf_ready == 0) {              //Wait on new data
    delay(10);
  }

  return true;                                    //True when finished
}

static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
  numpy::int16_to_float(&inference.buffer[offset], out_ptr, length);                      //get raw audio signal data

  return 0;
}

  

static void microphone_inference_end(void)                      //stop PDM and release buffers
{
  PDM.end();
  free(inference.buffer);
}
