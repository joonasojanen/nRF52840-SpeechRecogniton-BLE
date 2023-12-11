/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// If your target is limited in memory remove this macro to save 10K RAM
#define EIDSP_QUANTIZE_FILTERBANK   0

/**
 * Define the number of slices per model window. E.g. a model window of 1000 ms
 * with slices per model window set to 4. Results in a slice size of 250 ms.
 * For more info: https://docs.edgeimpulse.com/docs/continuous-audio-sampling
 */
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 4

/*
 ** NOTE: If you run into TFLite arena allocation issue.
 **
 ** This may be due to may dynamic memory fragmentation.
 ** Try defining "-DEI_CLASSIFIER_ALLOCATION_STATIC" in boards.local.txt (create
 ** if it doesn't exist) and copy this file to
 ** `<ARDUINO_CORE_INSTALL_PATH>/arduino/hardware/<mbed_core>/<core_version>/`.
 **
 ** See
 ** (https://support.arduino.cc/hc/en-us/articles/360012076960-Where-are-the-installed-cores-located-)
 ** to find where Arduino installs cores on your machine.
 **
 ** If the problem persists then there's not enough memory for this model and application.
 */

/* Includes ---------------------------------------------------------------- */
#include <PDM.h>
#include <miniproject2_v2_inferencing.h>
#include <ArduinoBLE.h>

// Set UUID for service and characteristic
BLEService randomService("d36d58de-dd08-41ff-9b86-5afd37baea97");
BLEUnsignedCharCharacteristic randomChar("cc3b0b57-5f84-49a0-acc6-e002fde01d80", BLERead | BLENotify);

// Define LED pins
#define RED_LED_PIN 11
#define BLUE_LED_PIN 12
#define GREEN_LED_PIN 13

/* threshold for predictions */
float threshold = 0.7;
bool isAdvertising = true;

/** 
  LABELS INDEX:
  0 - happy
  1 - noise
  2 - one
  3 - stop
  4 - three
  5 - two
  6 - unknown

*/
// LED pin (defines color) to light up
/**
 PIN 0  - OFF
 PIN 11 - RED
 PIN 12 - BLUE
 PIN 13 - GREEN
*/

int LED = 0;
int oldLED;

/** Audio buffers, pointers and selectors */
typedef struct {
    signed short *buffers[2];
    unsigned char buf_select;
    unsigned char buf_ready;
    unsigned int buf_count;
    unsigned int n_samples;
} inference_t;

static inference_t inference;
static bool record_ready = false;
static signed short *sampleBuffer;
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);

void setup()
{
  Serial.begin(115200);
  // comment out the below line to cancel the wait for USB connection (needed for native USB)
  //while (!Serial);

  // begin BLE initialization
  if (!BLE.begin()) {
    //Serial.println("Starting BLE failed!");

    while (1);
  }

  BLE.setLocalName("SpeechData");
  BLE.setAdvertisedService(randomService);
  randomService.addCharacteristic(randomChar);
  BLE.addService(randomService);

  BLE.advertise();

  //Serial.print("Advertised service UUID: ");
  //Serial.println(randomService.uuid());

  //Serial.println("Peripheral device active, waiting for central connection...");

  // summary of inferencing settings (from model_metadata.h)
  //ei_printf("Inferencing settings:\n");
  //ei_printf("\tInterval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
  //ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  //ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
  //ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) /
  //                                        sizeof(ei_classifier_inferencing_categories[0]));

  run_classifier_init();
  if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false) {
      //ei_printf("ERR: Could not allocate audio buffer (size %d), this could be due to the window length of your model\r\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
      return;
  }
}

void loop()
{
  if (!BLE.connected() && !isAdvertising) {
    //Serial.println("Connection to BLE failed!");
    BLE.advertise();
    isAdvertising = true;
  }
  else if (BLE.connected()) {
    BLEDevice central = BLE.central();

    bool m = microphone_inference_record();
    if (!m) {
        //ei_printf("ERR: Failed to record audio...\n");
        return;
    }

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
    signal.get_data = &microphone_audio_signal_get_data;
    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
    if (r != EI_IMPULSE_OK) {
        //ei_printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)) {
        // print the predictions
        //ei_printf("Predictions ");
        //ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        //    result.timing.dsp, result.timing.classification, result.timing.anomaly);
        //ei_printf(": \n");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            //ei_printf("    %s: %.5f\n", result.classification[ix].label,
            //          result.classification[ix].value);
            //lets light up some LEDS

            if (result.classification[ix].value > threshold) {
              //now let's see what label were in
              switch (ix) {
                case 2:
                  LED = 11;
                  if (central.connected()) {
                    randomChar.writeValue(ix);
                  }
                  break;
                case 5:
                  LED = 13;
                  if (central.connected()) {
                    randomChar.writeValue(ix);
                  }
                  break;
                case 4:
                  LED = 12;
                  if (central.connected()) {
                    randomChar.writeValue(ix);
                  }
                  break;
                case 0:
                  LED = 0;
                  if (central.connected()) {
                    randomChar.writeValue(ix);
                  }
                  break;
                default:
                  break;
              }
              //in Sense, LOW will light up the LED
              if (LED != 0) {
                digitalWrite (oldLED, HIGH); //if we enter a word right next to previous - we turn off the previous LED
                digitalWrite (LED, LOW);            
                oldLED = LED;
              }
              else //turn off LED
                digitalWrite (oldLED, HIGH);
            }
        }
  #if EI_CLASSIFIER_HAS_ANOMALY == 1
          //ei_printf("    anomaly score: %.3f\n", result.anomaly);
  #endif

          print_results = 0;
      }
  }
  if (BLE.connected() && isAdvertising) {
    //Serial.println("isAdvertising reset!");
    isAdvertising = false;
  }
}

/**
 * @brief      PDM buffer full callback
 *             Get data and call audio thread callback
 */
static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();

    // read into the sample buffer
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (record_ready == true) {
        for (int i = 0; i<bytesRead>> 1; i++) {
            inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];

            if (inference.buf_count >= inference.n_samples) {
                inference.buf_select ^= 1;
                inference.buf_count = 0;
                inference.buf_ready = 1;
            }
        }
    }
}

/**
 * @brief      Init inferencing struct and setup/start PDM
 *
 * @param[in]  n_samples  The n samples
 *
 * @return     { description_of_the_return_value }
 */
static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[0] == NULL) {
        return false;
    }

    inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[1] == NULL) {
        free(inference.buffers[0]);
        return false;
    }

    sampleBuffer = (signed short *)malloc((n_samples >> 1) * sizeof(signed short));

    if (sampleBuffer == NULL) {
        free(inference.buffers[0]);
        free(inference.buffers[1]);
        return false;
    }

    inference.buf_select = 0;
    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_inference_callback);

    PDM.setBufferSize((n_samples >> 1) * sizeof(int16_t));

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
        //ei_printf("Failed to start PDM!");
    }

    // set the gain, defaults to 20
    PDM.setGain(127);

    record_ready = true;

    return true;
}

/**
 * @brief      Wait on new data
 *
 * @return     True when finished
 */
static bool microphone_inference_record(void)
{
    bool ret = true;

    if (inference.buf_ready == 1) {
        //ei_printf(
        //    "Error sample buffer overrun. Decrease the number of slices per model window "
        //    "(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
        ret = false;
    }

    while (inference.buf_ready == 0) {
        delay(1);
    }

    inference.buf_ready = 0;

    return ret;
}

/**
 * Get raw audio signal data
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);

    return 0;
}

/**
 * @brief      Stop PDM and release buffers
 */
static void microphone_inference_end(void)
{
    PDM.end();
    free(inference.buffers[0]);
    free(inference.buffers[1]);
    free(sampleBuffer);
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif
