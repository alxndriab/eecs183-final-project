#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// globla constant for the width/height of LED screen
//*********************** was this added later by choyce?
const int LED_HEIGHT = 16; 
const int LED_WIDTH = 32; 

const int INVADER_STENCIL[4][4] = { {0, 1, 1, 0}, 
                                    {1, 2, 2, 1}, 
                                    {1, 1, 1, 1}, 
                                    {1, 0, 0, 1}};

const int PLAYER_STENCIL[2][3] = { {0, 1, 0}, 
                                   {1, 1, 1}};

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      strength = 0;
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
       x = x_arg;
       y = y_arg;
       strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
       x = x_arg;
       y = y_arg;
       strength = strength_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      y -= 1;
    }
    

    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength <= 0) {
        draw_with_rgb(BLACK, BLACK);
      }
      if (strength == 1) {
        draw_with_rgb(RED, BLUE);
      }
      else if (strength == 2) {
        draw_with_rgb(ORANGE, BLUE);
      }
      else if (strength == 3) {
        draw_with_rgb(YELLOW, BLUE);
      }
      else if (strength == 4) {
        draw_with_rgb(GREEN, BLUE);
      }
      else if (strength == 5) {
        draw_with_rgb(BLUE, BLUE);
      }
      else if (strength == 6) {
        draw_with_rgb(PURPLE, BLUE);
      }
      else if (strength == 7) {
        draw_with_rgb(WHITE, BLUE);
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
       draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      strength--;
      erase();
      draw();
    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      //left top corner point is default xy point
      //******************changed from int i = 0 and int j = 0 and i < 4 and j < 4 to int i = x and int j = y and i < x + 4 and j < y + 4 so that the xy coordinates are applied to the placement of invader
      for (int i = x; i < x + 4; i++) {
        for (int j = y; j < y + 4; j++) {
          if (INVADER_STENCIL[j][i] == 0) {
            matrix.drawPixel(j, i, BLACK.to_333());
          }
          else if (INVADER_STENCIL[i][j] == 1) {
            matrix.drawPixel(j, i, body_color.to_333());
          }
          else {
            matrix.drawPixel(j, i, eye_color.to_333());
          }
          //*********below only for testing purposes - TO BE DELETED
          Serial.print(y);
          Serial.print(",");
          Serial.print(x);
          Serial.print(" ");
          Serial.println(INVADER_STENCIL[y][x]);
        }
      }

    }
};

class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      fired = true;
      y += 1;
      if (y > LED_HEIGHT) {
        reset();
      }
    }
    
    // resets private data members to initial values
    void hit() {
      reset();
    }
    
    // draws the Cannonball, if it is fired

    //*********** was draw_with_rgb() but cannot use that because that is private member function in Invader class
    void draw() {
      if (fired) {
        matrix.drawPixel(x, y, ORANGE.to_333());
        matrix.drawPixel(x + 1, y, ORANGE.to_333());
      }
    }
    
    // draws black where the Cannonball used to be
    //*************the same applies here
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
};

class Player {
  public:
    Player() {
      //*********** should the player initialization be set to the bottom of the pixel page? as in y = 15
      x = 0;
      y = 15;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }
    
    // Modifies: lives
    void die() {
      lives--;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    //************** changed a bit - cannot use same format as previous draw_with_rgb because need to create a new hardcoded STENCIL for player (seen above)
    void draw_with_rgb(Color color) {
      for (int i = x; i < x + 3; i++) {
        for (int j = y; j < y + 3; j++) {
          if (PLAYER_STENCIL[i][j] == 0) {
            matrix.drawPixel(j, i, BLACK.to_333());
          }
          else {
            matrix.drawPixel(j, i, color.to_333());
          }
          //**********below purely for testing purposes - TO BE DELETED
          Serial.print(y);
          Serial.print(",");
          Serial.print(x);
          Serial.print(" ");
          Serial.println(PLAYER_STENCIL[y][x]);
        }
      }
    }
};

class Game {
  public:
    Game() {
      level = 0;
      time = 0;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setupGame() {
//i dont have this stuff in my code, but i dont want to get rid of it yet just in case
 /*     //**********this turns on the display
      matrix.begin();
      //********* this clears the display initially
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 16; j++) {
          matrix.drawPixel(i, j, BLACK.to_333());
        }
      }
     */
      
matrix.fillScreen(BLACK.to_333());
int x = 0;
int y = 0;

      if(level == 1){
        print_level(level);
        delay(3000);
        matrix.fillScreen(BLACK.to_333());
        for(int i = 0; i < NUM_ENEMIES / 2; i++){
          enemies[i].initialize(x, y, 1);
          x += 4;
        }
      }

      if(level == 2){
        print_level(level);
        delay(3000);
        matrix.fillScreen(BLACK.to_333());
        for(int i = 0; i < NUM_ENEMIES; i++){
          enemies[i].initialize(x, y, 1);
          x += 4;
          if(i = NUM_ENEMIES / 2){
            y += 8;
          }
        }
        for(int i = 1; i < NUM_ENEMIES / 2; i += 2){
        enemies[i].initialize(x, y, 2);
          x += 4;
       }
         y += 8;
        for(int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i += 2){
          enemies[i].initialize(x, y, 2);
          x += 4;
        }
      }

    if(level == 3){
       print_level(level);
        delay(3000);
        matrix.fillScreen(BLACK.to_333());
      for(int i = 0; i < NUM_ENEMIES ; i++){
       int j = (i + 1) % 5;
        enemies[i].initialize(x, y, j);
        x += 4;
        if(i = NUM_ENEMIES / 2){
          y += 8;
        }
      }
    }

    if(level == 4){
       print_level(level);
       delay(3000);
       matrix.fillScreen(BLACK.to_333());
    for(int i = 0; i < NUM_ENEMIES / 2; i++){
      if(i % 2 == 0){
        enemies[i].initialize(x, y, 5);
      }
      else{
        enemies[i].initialize(x, y, 4);
      }
      x += 4;
    }
    y += 8;
    for(int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++){
      if(i % 2 == 0){
        enemies[i].initialize(x, y, 2);
      }
      else{
        enemies[i].initialize(x, y, 3);
      }
      x += 4;
    }
    }

    if(level > 4){
       print_level(level);
       delay(3000);
       matrix.fillScreen(BLACK.to_333());
      for(int i = 0; i < NUM_ENEMIES ; i++){
       int j = random(1, 8);
        enemies[i].initialize(x, y, j);
        x += 4;
        if(i = NUM_ENEMIES / 2){
          y += 8;
        }
      }
    }

/*      //********* this clears the display - again, after printing the level
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 16; j++) {
          matrix.drawPixel(i, j, BLACK.to_333());
        }
      }
*/
      //********** this sets the invader strengths and their xy coordinates
      //***********this is only for level 1 (because strength is 1 for all invaders)
      x = 0;
      y = 0;
      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].initialize(x, y, 1);
        // ************* want the invader in a horizontal line, so must be side by side each other 
        x += 4; 
      }
      //********** this draws the invaders 
      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].draw();
      }

      //******** this draws the player in the bottom middle location
      player.set_x(16);
      player.draw();

    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix

    //this is the main function
    void update(int potentiometer_value, bool button_pressed) {
      //******** this below is for testing purposes only - TO BE DELETED 
      Invader i1(1, 2, 1);
      i1.draw();

      //this below is the actual operation



    }

  private:
    int level;
    unsigned long time;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      //******** struggling to find a way to detect that all invader is defeated; one way is to detect if all the pixel in matrix is BLACK - but don't know where to get the color of the pixels in matrix...
    }

    // set up a level
    void reset_level() {

    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.update(potentiometer_value, button_pressed);
}

// displays Level
void print_level(int level) {
  //********* im messing with the font color and size and the cursor of the level - idk if there is a standard display image
  matrix.setCursor(8, 10);
  matrix.setTextColor(WHITE.to_333());
  matrix.setTextSize(5);

  matrix.print("Level ");
  matrix.print(level);
}

// displays number of lives
void print_lives(int lives) {
  //********** again same here I'm messing with the cursor for displaying number of lives
  matrix.setCursor(0, 31); 
  matrix.setTextColor(WHITE.to_333());
  matrix.setTextSize(5);

  matrix.print("Lives: ");
  matrix.print(lives);
}

// displays "game over"
void game_over() {
  //****** messing again - same setting as print_level 
  matrix.setCursor(8, 10);
  matrix.setTextColor(WHITE.to_333());
  matrix.setTextSize(5);

  matrix.print("Game Over");
}
