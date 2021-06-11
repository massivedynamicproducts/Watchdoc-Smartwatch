#include "Adafruit_Arcada.h"
Adafruit_Arcada arcada;

//Temp & Ox Reading Related
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30100_PulseOximeter.h"
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
//SPO2 Things
#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;
uint32_t tsLastReport = 0;
int second_in = 0;
float temp_temp = 0;
int temp_ox = 0;




#include "welcome_screen.h"
#include "self_check_title.h"









unsigned long previousMillis = 0;        // will store last time updated
unsigned long previousMillis2 = 0;        // will store last time updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
const long interval = 1000;           // interval at which to blink (milliseconds)
const long interval2 = 1000;           // interval at which to blink (milliseconds)











#include <math.h>
#include <PDM.h>
#include <EloquentTinyML.h>      
#include "tf_lite_model.h"       // TF Lite model file

#define PDM_SOUND_GAIN     255   // sound gain of PDM mic
#define PDM_BUFFER_SIZE    256   // buffer size of PDM mic

#define SAMPLE_THRESHOLD   100   // RMS threshold to trigger sampling
#define FEATURE_SIZE       64    // sampling size of one voice instance
#define SAMPLE_DELAY       10    // delay time (ms) between sampling

#define NUMBER_OF_LABELS   2     // number of voice labels

#define PREDIC_THRESHOLD   0.6   // prediction probability threshold for labels
#define RAW_OUTPUT         true  // output prediction probability of each label
#define NUMBER_OF_INPUTS   FEATURE_SIZE
#define NUMBER_OF_OUTPUTS  NUMBER_OF_LABELS
#define TENSOR_ARENA_SIZE  4 * 1024


Eloquent::TinyML::TfLite<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> tf_model;
float feature_data[FEATURE_SIZE];
volatile float rms;
bool voice_detected;





int coughs = 0;






float temp_var = 0;

int   temp_var2 = 0;








  const int buttonB = 11;
  
  int health_app = 1;
  
  int self_checker = 0;
  
  int question_no = 0;
  int yes = 0;
  int no = 0;

void setup() {

    
  // put your setup code here, to run once:
  Serial.begin(115200);


  delay(5000);
  
  PDM.onReceive(onPDMdata);
  PDM.setBufferSize(PDM_BUFFER_SIZE);
  PDM.setGain(PDM_SOUND_GAIN);

  if (!PDM.begin(1, 16000)) {  // start PDM mic and sampling at 16 KHz
    Serial.println("Error: PDM not initialized.");
    while (1);  //Don't Go Any Further
  }

  arcada.arcadaBegin();
  arcada.displayBegin();
  if (! arcada.createFrameBuffer(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT)) {
    Serial.print("Failed to allocate framebuffer");
    while (1);
  }



  // Display Welcome Screen
  GFXcanvas16 *canvas = arcada.getCanvas();
  uint16_t *buffer = canvas->getBuffer();
  for (int h=0; h<240; h++) {
    for (int w=0; w<WELCOME_SCREEN_HEIGHT; w++) {
      buffer[h*WELCOME_SCREEN_HEIGHT + w] = welcome_screen[h*WELCOME_SCREEN_HEIGHT + w];
    }
  } 

  //captured frame's starting x,y coordinates anywhere on screen
  arcada.blitFrameBuffer(0, 0);
  // Turn on backlight
  for (int i=0; i<=64; i++) {
    arcada.setBacklight(i);
    delay(10);
  }
  
  
  
  pinMode(LED_BUILTIN, OUTPUT);

  // wait 1 second to avoid initial PDM reading
  delay(1000);

  // start TF Lite model
  tf_model.begin((unsigned char*) model_data);
  
  

  arcada.display->fillScreen(ARCADA_BLACK);



  if (!pox.begin()) {
      Serial.println("FAILED");
      for(;;);
  } else {
      Serial.println("SUCCESS");
  }
  
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  //Temp Sensor Setup
  mlx.begin();  
     
}

void loop() {

  
//TODO: ADD COUGHDETECTION FUNCTION IN THE MAIN MENU(INSIDE LOOP while( if not health_app or self_checker ) display 2 app logos 
//Maybe use buzzer when temp. high????







  
  
  arcada.readButtons();
  //uint8_t justPressed = arcada.justPressedButtons();
  uint8_t justReleased = arcada.justReleasedButtons();
  if (justReleased & ARCADA_BUTTONMASK_LEFT) {
    Serial.print("Health App. \n " );
    arcada.display->fillScreen(ARCADA_BLACK);
    
    health_app = 1;
    
  }

  if (justReleased & ARCADA_BUTTONMASK_RIGHT) {
  Serial.print("Self Checker. \n " );
  arcada.display->fillScreen(ARCADA_BLACK);

     /* 
        // Display Welcome Self Checker
        GFXcanvas16 *canvas2 = arcada.getCanvas();
        uint16_t *buffer2 = canvas2->getBuffer();
        for (int h=0; h<240; h++) {
          for (int w=0; w<SELF_CHECK_HEIGHT; w++) {
            buffer2[h*SELF_CHECK_HEIGHT + w] = self_check_logo[h*SELF_CHECK_HEIGHT + w];
          }
        } 
      
        //captured frame's starting x,y coordinates anywhere on screen
        arcada.blitFrameBuffer(0, 0);

        delay(1000);
*/
   arcada.display->fillScreen(ARCADA_BLACK);

   
  self_checker = 1;
    
  }  





  while ( health_app == 1 )
  {
     pox.update();
     int ox = pox.getSpO2();
     float temp = mlx.readObjectTempC();


     
  arcada.display->setCursor(20, 10);
  arcada.display->setTextColor(ARCADA_RED);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print("Symptom Detector");


       
  arcada.display->setCursor(20, 50);
  arcada.display->setTextColor(ARCADA_WHITE);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print("Temperature:");

  arcada.display->setCursor(20, 110);
  arcada.display->setTextColor(ARCADA_WHITE);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print("SPO2:      %");
  
    Cough_Detection();
    
        if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    
        Serial.print("bpm / SpO2:");
        Serial.print(ox);
        Serial.println("%");

        Serial.print("*C\tObject = "); Serial.print(temp); Serial.println("*C");
        
        Serial.println();
 
        tsLastReport = millis();

        if ( second_in == 0 )
        {
          if( ox > 50 )
          {
            arcada.display->setCursor(180, 110);
            arcada.display->setTextColor(ARCADA_WHITE);
            arcada.display->setTextWrap(true);
            arcada.display->setTextSize(2);
            arcada.display->print(ox);
          }

          
            arcada.display->setCursor(180, 50);
            arcada.display->setTextColor(ARCADA_WHITE);
            arcada.display->setTextWrap(true);
            arcada.display->setTextSize(2);
            arcada.display->print(temp);

            if (temp > 38)
            {
              tone(46, 147, 1000);
            }
            
            
            temp_temp = temp;
            temp_ox = ox;
            second_in = 1;
        }
        else
        {
          if( temp_ox > 50 )
          {
            arcada.display->setCursor(180, 110);
            arcada.display->setTextColor(ARCADA_BLACK);
            arcada.display->setTextWrap(true);
            arcada.display->setTextSize(2);
            arcada.display->print(temp_ox);

            //if (50 < ox < 90)
            //{
            //  tone(46, 147, 1000);
            //}
          }
          
          arcada.display->setCursor(180, 50);
          arcada.display->setTextColor(ARCADA_BLACK);
          arcada.display->setTextWrap(true);
          arcada.display->setTextSize(2);
          arcada.display->print(temp_temp);

          tsLastReport = -REPORTING_PERIOD_MS;
          second_in = 0;
        }
        
        
    }
    

    display_coughs( ARCADA_WHITE );
    
    attachInterrupt(digitalPinToInterrupt(buttonB), buttonB_Handler, FALLING);

  }












  while( self_checker == 1)
  {
  
        
        if( question_no < 9 )
        {
          arcada.display->setCursor(30, 10);
          arcada.display->setTextColor(ARCADA_RED);
          arcada.display->setTextWrap(true);
          arcada.display->setTextSize(3);
          arcada.display->print("Question");
  
          arcada.display->setCursor(190, 10);
          arcada.display->setTextColor(ARCADA_RED);
          arcada.display->setTextWrap(true);
          arcada.display->setTextSize(3);
          arcada.display->print(question_no);
        }
    //Cough_Detection(); cough detection delays of microphone makes this wait the whole self checker app , get rid of delays if possible, if not dont use here
        
        if ( question_no == 0 )
        {
          arcada.display->fillScreen(ARCADA_BLACK);
          question_no == 1;
        }
                
        if ( question_no == 1 )
          display_question("Do you have any chest pain or pressure?", ARCADA_WHITE);
        if ( question_no == 2 )
          display_question("Do you experience any difficulties while walking or speaking?", ARCADA_WHITE);
        if ( question_no == 3 )
          display_question("Have you experienced loss of taste or smell recently?", ARCADA_WHITE);
        if ( question_no == 4 )
          display_question("Do you feel tired?", ARCADA_WHITE);
        if ( question_no == 5 )
          display_question("Do you feel any pain in your body?", ARCADA_WHITE);
        if ( question_no == 6 )
          display_question("Do you have sore throat?", ARCADA_WHITE);
        if ( question_no == 7 )
          display_question("Do you have diarrhea?", ARCADA_WHITE);
        if ( question_no == 8 )
          display_question("Do you have conjunctivitis? (Pink Eye) ", ARCADA_WHITE);
        if ( question_no == 9 )
        {
          if ( yes >= 6 )
            arcada.display->fillScreen(ARCADA_RED);
          else
            arcada.display->fillScreen(ARCADA_GREEN);
          
          arcada.display->setCursor(10, 60);
          arcada.display->setTextColor(ARCADA_BLACK);
          arcada.display->setTextWrap(true);
          arcada.display->setTextSize(2);
          if ( yes >= 6 )
          {
            arcada.display->print("Seek medical \n attention.");
          }
          else
          {
            arcada.display->print("No major symptoms \n detected.");
          }
          self_checker = 0;
          health_app = 1;
          delay(5000);
          arcada.display->fillScreen(ARCADA_BLACK);  //from here return back to main menu & reset the variables
          question_no = 0;
          yes = 0;
          no = 0;
        }

           
        arcada.readButtons();
        //uint8_t justPressed = arcada.justPressedButtons();
        uint8_t justReleased = arcada.justReleasedButtons();
        if (justReleased & ARCADA_BUTTONMASK_LEFT) {
          Serial.print("Yes. \n " );
      
          if ( question_no == 1 || question_no == 2 )
            yes = yes+5;
          if ( question_no == 3)
            yes = yes+3;
          else
            yes++;
            
          question_no++;
      
          arcada.display->fillScreen(ARCADA_BLACK);
        }
      
          if (justReleased & ARCADA_BUTTONMASK_RIGHT) {
          Serial.print("No. \n " );
      
          no++;
      
          question_no++;
      
          arcada.display->fillScreen(ARCADA_BLACK);
        }
  }
//END OF SELF_CHECKER PART





}






void display_temp(uint16_t color) {

  float temperature = mlx.readObjectTempC();
  pox.update();
         
  unsigned long currentMillis = millis();
  
  
  
  arcada.display->setCursor(20, 50);
  arcada.display->setTextColor(color);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print("Temperature:");

  arcada.display->setCursor(20, 110);
  arcada.display->setTextColor(color);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print("SPO2:      %");
  
  if(currentMillis - previousMillis >= interval) {

    

    
    // save the last time you get in here
    previousMillis = currentMillis;   
  
    arcada.display->setCursor(180, 50);
    arcada.display->setTextColor(ARCADA_BLACK);
    arcada.display->setTextWrap(true);
    arcada.display->setTextSize(2);
    arcada.display->print(temp_var);

    temp_var = temperature;

    arcada.display->setCursor(180, 110);
    arcada.display->setTextColor(ARCADA_BLACK);
    arcada.display->setTextWrap(true);
    arcada.display->setTextSize(2);
    arcada.display->print(pox.getSpO2());

   
  }
  //delay(1000);
  else
  {
    
    
    arcada.display->setCursor(180, 50);
    arcada.display->setTextColor(color);
    arcada.display->setTextWrap(true);
    arcada.display->setTextSize(2);
    arcada.display->print(temp_var);

    arcada.display->setCursor(180, 110);
    arcada.display->setTextColor(color);
    arcada.display->setTextWrap(true);
    arcada.display->setTextSize(2);
    arcada.display->print(pox.getSpO2());
  }
  

}










void display_coughs(uint16_t color) {

  unsigned long currentMillis2 = millis();

  arcada.display->setCursor(20, 80);
  arcada.display->setTextColor(color);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print("Total Coughs:");  

  if(currentMillis2 - previousMillis2 >= interval2) {
    // save the last time you get in here
    previousMillis2 = currentMillis2;   

    arcada.display->setCursor(180, 80);
    arcada.display->setTextColor(ARCADA_BLACK);
    arcada.display->setTextWrap(true);
    arcada.display->setTextSize(2);
    arcada.display->print(temp_var2);

    temp_var2 = coughs;
    
  }
  //delay(1000);
  else
  {

    arcada.display->setCursor(180, 80);
    arcada.display->setTextColor(color);
    arcada.display->setTextWrap(true);
    arcada.display->setTextSize(2);
    arcada.display->print(temp_var2);

  }
  

}



















void display_question(char *text, uint16_t color) {
  arcada.display->setCursor(10, 60);
  arcada.display->setTextColor(color);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  arcada.display->print(text);
}














// Screen/Device Turn On Off Button
void buttonB_Handler() 
{
  if ( health_app == 1 ) 
  {
    
    health_app = 0;
    self_checker = 1;
  }
  
}








// callback function for PDM mic
void onPDMdata() {

  rms = -1;
  short sample_buffer[PDM_BUFFER_SIZE];
  int bytes_available = PDM.available();
  PDM.read(sample_buffer, bytes_available);

  // calculate RMS (root mean square) from sample_buffer
  unsigned int sum = 0;
  for (unsigned short i = 0; i < (bytes_available / 2); i++) sum += pow(sample_buffer[i], 2);
  rms = sqrt(float(sum) / (float(bytes_available) / 2.0));
}





void Cough_Detection()
{
  
  // Sesin Eşik Değeri Aşmasını Bekle
  if (rms > SAMPLE_THRESHOLD)
  {
    //  Aşınca turuncu led on & Sampling Start
    digitalWrite(LED_BUILTIN, HIGH);
    for (int i = 0; i < FEATURE_SIZE; i++) {  // sampling
      while (rms < 0);
      feature_data[i] = rms;  
      delay(SAMPLE_DELAY);
    }
    digitalWrite(LED_BUILTIN, LOW); //Sampling Bitince Led Off
  
    // predict voice and put results (probability) for each label in the array
    float prediction[NUMBER_OF_LABELS];
    tf_model.predict(feature_data, prediction);
  
    voice_detected = false;
      if (prediction[0] >= PREDIC_THRESHOLD) {
        coughs++;
        Serial.print(coughs);
        Serial.print("\n");
        voice_detected = true;

        if (coughs > 10)
        {
          tone(46, 147, 1000);
        }
      }
    // wait for 1 second after one sampling/prediction
    delay(1000);
  }
  

  
}
