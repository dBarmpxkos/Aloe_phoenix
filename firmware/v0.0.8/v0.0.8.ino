#include <FastLED.h>

/* FastLED */
FASTLED_USING_NAMESPACE

#define DATA_PIN    7
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    8
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  200
/* FastLED */


/* Variables */
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
boolean newData = false;

uint8_t redValue, greenValue, blueValue;
/* Variables */


/* Functions */
void rec_with_start_end_markers() {

	static boolean recvInProgress = false;
	static byte ndx = 0;
	char startMarker = '<';
	char endMarker = '>';
	char rc;

	while (Serial.available() > 0 && newData == false) {
		rc = Serial.read();

		if (recvInProgress == true) {
			if (rc != endMarker) {
				receivedChars[ndx] = rc;
				ndx++;
				if (ndx >= numChars) {
					ndx = numChars - 1;
				}
			}
			else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
        	recvInProgress = true;
        }
    }   
}

void parse_data() { 	// split the data into its parts

    char * strtokIndx;	// this is used by strtok() as an index

    strtokIndx = strtok(tempChars, ",");
    redValue = atoi(strtokIndx);
    
    strtokIndx = strtok(NULL, ",");
    greenValue = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");  
    blueValue = atoi(strtokIndx);
}


// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
	if( cur == target) return;

	if( cur < target ) {
		uint8_t delta = target - cur;
		delta = scale8_video( delta, amount);
		cur += delta;
		} else {
			uint8_t delta = cur - target;
			delta = scale8_video( delta, amount);
			cur -= delta;
		}
	}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
	nblendU8TowardU8( cur.red,   target.red,   amount);
	nblendU8TowardU8( cur.green, target.green, amount);
	nblendU8TowardU8( cur.blue,  target.blue,  amount);
	return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t start, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
	for( uint16_t i = start; i < N+1; i++) {
		fadeTowardColor( L[i], bgColor, fadeAmount);
	}
}

void color_fade_sweep(CRGB* L, RGB_LoadStruct *RGB_LOAD, const CRGB& targetColor){
	for (int i=0; i<255; i++){
		fadeTowardColor( L, RGB_LOAD->startLedNumber, RGB_LOAD->endLedNumber, targetColor, 2);
		FastLED.show();
		delay(20);
	}	
}
/* Functions */


void setup() {
	delay(1000);
	Serial.begin(9600);
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	CRGB targetColor(180, 180, 180);
   	color_fade_sweep(leds, RGB_LOAD, targetColor);
}


void loop()
{
	rec_with_start_end_markers();

	if (newData == true) {
		strcpy(tempChars, receivedChars);
		parse_data();
        CRGB targetColor(redValue, greenValue, blueValue);
    	color_fade_sweep(leds, RGB_LOAD, targetColor);
   	    FastLED.show();

   	    newData = false;

    }

}