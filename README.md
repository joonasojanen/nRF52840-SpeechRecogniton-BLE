This project demonstrates a simple speech recognition system controlling an ESP32 through a Bluetooth Low Energy Human Interface Device (BLE HID) profile. The speech recognition model is developed using Edge Impulse and deployed on the Seeed Xiao BLE nRF52840 Sense board. In this project we used the SoundxVision ring equipped with the nRF62840 board. The ESP32 board also has an RGB LED and an LCD screen connected to it. The LCD screen provides real-time feedback on the status of the Bluetooth connection.

#### Speech Commands

- **"one":** Changes the LED to red.
- **"two":** Changes the LED to green.
- **"three":** Changes the LED to blue.
- **"happy":** Turns off the LED.

## Contents

#### [1. Setup Edge Impulse library](#setup-edge-impulse-and-create-ml-model-arduino-library-for-speech-recognition)
#### [2. Setup ArduinoIDE](#setup-arduinoide)
#### [3. Troubleshooting](#troubleshooting)

---

Here is a quick guide on how to use and implement this project:

### Setup Edge Impulse and create ML model Arduino library for speech recognition

1. Create an Edge Impulse account: https://edgeimpulse.com/
2. Download datasets, in this case we use Google Speech commands dataset and keywords dataset.
   
   a. Google Speech commands dataset can be downloaded from this link:
   https://storage.googleapis.com/download.tensorflow.org/data/speech_commands_v0.02.tar.gz From this dataset we use words/folders “one”, “two”, “three”, “happy” and “stop”.

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/e2211aa3-76e2-4f13-9cda-33a267c09062)

   b. Keywords dataset can be downloaded from this link:
   https://docs.edgeimpulse.com/docs/pre-built-datasets/keyword-spotting From this data set we use words/folders “unknown” and “noise”.

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/141937a1-2a64-4f38-9c05-f0e116b3bcde)

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/a1342f01-719f-4e6e-8f0b-0fb50601995f)

3. Open Edge Impulse and create a new project:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/4cbafda3-28e1-4ea0-9d06-c7ddbc76d9c9)

4. Go to the data acquisition page:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/f85830ce-86ee-436e-926f-46addb80cf1f)

   Select Add existing data, and then Upload data:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/df5f5fd4-dd76-4c44-afc7-ef48422f31af)

   Choose folder "one" from the Google Speech commands dataset. In the Label section enter label "one", and press Upload data:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/76ed28b4-e8c6-4b0a-b2ac-734663e9ca68)

   Once it is done it should show "Job completed":

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/75e65e5a-2473-4aa7-86b6-d63b5b5e7ff2)

   You need to upload the rest of the words by using the small upload button on the dataset. Repeat the above process for "two", "three", "happy" and "stop" from Google speech commands dataset, and "unknown" and "noise" from keywords dataset. Remember to change the label to match each word:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/b11f97a2-aef7-4506-aaaf-5ab353eb6f62)

   After uploading each folder, you should see all the labels from the "filter your data" tab:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/60bcb9e0-e42b-422e-b286-717618eea046)

5. Create impulse by pressing Create impulse on the left hand side:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/5479a188-ead4-4807-a6ef-4a123036d559)

   Next you need to add a processing block Audio (MFCC), then add a learning block Classification, and then Save Impulse:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/c1b20c7e-d8c8-4608-ac0e-478e252cf607)

6. Go to the MFC from the tab on the left and Save parameters with the default parameters, and then Generate features:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/3cafc424-d201-43af-9798-0d9f2f3114b4)

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/d81003ce-a298-402a-a6b0-71b4cec01b1f)

   After the features are generated, the results should look something like this:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/15aa597e-13d6-4073-b493-d8786b632be6)

7. Go to the Classifier from the tab on the left and start the training. We can use the default parameters:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/9aeceaab-086a-40b9-ac2f-1672d2337b80)

   Select the Target device from the top right corner while the training is running, and select nRF52840:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/fc55f7d7-f571-41ea-9c4e-3f815c7c3015)

   After the training is done, the results should look something like this:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/87a8b502-d8d0-4a01-a1b1-3105b341b3c9)

8. Go to the Model testing from the tab on the left and press Classify all:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/e5d32706-28b4-46df-bf8e-2c3d4a96d35f)

   The results should give something around 80% total accuracy. The individual words like "one", "two" and three" needed for this project have a better accuracy, so the results should be good enough for this project:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/a73adb1d-07e5-4619-9e52-e6ca462277be)

9. Lastly, download the Arduino library from the Deployment tab. Select the Arduino library from the "search deployment options" and press Build to download the library:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/558f9492-b9b7-44de-917c-831373701b73)

### Setup ArduinoIDE

1. Download ArduinoIDE: https://www.arduino.cc/en/software
2. Add the library that we downloaded from Edge Impulse by going to Sketch -> Include Library -> Add .ZIP Library and select the downloaded library:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/fbd45410-29a7-4251-b74a-50d11ae2504c)

3. Add nRF52840 and ESP32 boards by going to File -> Preferences... and add these links to Additional Boards Manager URLs:

   https://dl.espressif.com/dl/package_esp32_index.json

   https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/e7928a72-25d7-4f4b-8779-76020eaa293c)

   Then go to Tools -> Board -> Boards manager... and install Seeed nRF52 mbed-enabled Boards and esp32:

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/3f7b859b-205e-47da-bdad-2fc4ca1dcfa7)

4. Lastly, download the required libraries by going to Sketch -> Include Library -> Manage Libraries... and download ArduinoBLE and LiquidCrystal.

   ![kuva](https://github.com/LasseRapo/nRF52840-SpeechRecogniton-BLE/assets/71126392/bfb8f7b0-797d-4c64-9d0d-93f2f5d70be8)

### Troubleshooting
