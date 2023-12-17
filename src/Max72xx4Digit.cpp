#include "Max72xx4Digit.h"

static Max72xxPanel matrix = Max72xxPanel(PIN_CS, NUMBER_OF_HORIZONTAL_DISPLAYS, NUMBER_OF_VERTICAL_DISPLAYS);

//Week Days
static String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names
static String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

static int wait = 100; // In milliseconds
static int spacer = 1;
static int width  = 5 + spacer; // The font width is 5 pixels
static bool DisplayDot = true;

void SetupDisplay(){
    matrix.setIntensity(DAY_DISPLAY_INTENSITY);
    matrix.setRotation(0, 3);
    matrix.setRotation(1, 3);
    matrix.setRotation(2, 3);
    matrix.setRotation(3, 3);

    matrix.setPosition(0, 3, 0);
    matrix.setPosition(1, 2, 0);
    matrix.setPosition(2, 1, 0);
    matrix.setPosition(3, 0, 0);

    matrix.fillScreen(LOW);
    matrix.write();
}

void DisplayMessage(String message){
    for ( unsigned int i = 0 ; i < width * message.length() + matrix.width() - spacer; i++ ) {
        //matrix.fillScreen(LOW);
        unsigned int letter = i / width;
        int x = (matrix.width() - 1) - i % width;
        int y = (matrix.height() - 8) / 2; // center the text vertically

        while ( x + width - spacer >= 0 && letter >= 0 ) {
            if ( letter < message.length() ) {
                matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background off, reverse to invert the image
            }
            letter--;
            x -= width;
        }

        matrix.write(); // Send bitmap to display
        delay(wait/2);
    }
}

void DisplayClock(DateTime dateTime){
    int tempDigit;

    matrix.shutdown(true);
    matrix.fillScreen(LOW);
    matrix.shutdown(false);
    matrix.fillScreen(LOW);

    // First Digit
    tempDigit = dateTime.hour() / 10;
    if(tempDigit > 0){
        matrix.drawBitmap(0, 0, CF_LEFT[tempDigit], 8, 8, 1);
    }

    // Second Digit
    tempDigit = dateTime.hour() % 10;
    matrix.drawBitmap(7, 0, CF_LEFT[tempDigit], 8, 8, 1);

    // Display Colon
    if(DisplayDot){
        matrix.fillRect(15, 1, 2, 2, 1);
        matrix.fillRect(15, 6, 2, 2, 1);
    }

    // Third Digit
    tempDigit = dateTime.minute() / 10;
    matrix.drawBitmap(17, 0, CF_RIGHT[tempDigit], 8, 8, 1);

    // Fourth Digit
    tempDigit = dateTime.minute() % 10;
    matrix.drawBitmap(24, 0, CF_RIGHT[tempDigit], 8, 8, 1);

    matrix.write(); // Send bitmap to display

    if (DisplayDot == true){
        DisplayDot = false;
    }
    else{
        DisplayDot = true;
    }
}

void DisplayDate(DateTime dateTime){
    String FullDate;

    FullDate += weekDays[dateTime.dayOfTheWeek()];
    FullDate += ", ";
    FullDate += dateTime.day();
    FullDate += " ";
    FullDate += months[dateTime.month() - 1];
    FullDate += " ";
    FullDate += dateTime.year();

    DisplayMessage(FullDate);
}

void AdjustMax72XXBrightness(DateTime dateTime){
    int Hours = dateTime.hour();

    if (Hours > 17 || Hours < 6){
        matrix.setIntensity(NIGHT_DISPLAY_INTENSITY);
    }
    else{  
        matrix.setIntensity(DAY_DISPLAY_INTENSITY);
    }
}
