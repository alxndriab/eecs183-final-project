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
      y++;
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
      else if (strength >= 7) {
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

      for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
          if (INVADER_STENCIL[row][col] == 0) {
            matrix.drawPixel(col + x, row + y, BLACK.to_333());
          }
          else if (INVADER_STENCIL[row][col] == 1) {
            matrix.drawPixel(col + x, row + y, body_color.to_333());
          }
          else {
            matrix.drawPixel(col + x, row + y, eye_color.to_333());
          }
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
      y--;
      if (y < 0) {
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
        matrix.drawPixel(x, y + 1, ORANGE.to_333());
      }
    }
    
    // draws black where the Cannonball used to be
    //*************the same applies here
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x, y + 1, BLACK.to_333());
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
      x = 15;
      y = 14;
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
      // for (int i = 0; i < 3; i++) {
      //   for (int j = 0; j < 3; j++) {
      //     if (PLAYER_STENCIL[i][j] == 0) {
      //       matrix.drawPixel(j + x, i + y, BLACK.to_333());
      //     }
      //     else {
      //       matrix.drawPixel(j + x, i + y, color.to_333());
      //     }
      //   }
      // }

      for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 3; col++) {
          if (PLAYER_STENCIL[row][col] == 0) {
            matrix.drawPixel(col + x, row + y, BLACK.to_333());
          }
          else {
            matrix.drawPixel(col + x, row + y, color.to_333());
          }
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
      //! MOVE LEVEL SOMEWHERE ELSE
      level = 100;

      matrix.fillScreen(BLACK.to_333());
      int x = 0;
      int y = 0;

      Serial.println("Inside setupGame()");
      Serial.println(level);

      print_level(level);
      print_lives(player.get_lives());

      if(level == 1){
        Serial.println("Inside level 1");

        for(int i = 0; i < NUM_ENEMIES / 2; i++){
          enemies[i].initialize(x, y, 1);
          enemies[i].draw();
          x += 4;
        }
      }

      if(level == 2){
        for(int i = 0; i < NUM_ENEMIES / 2; i++){
          if(i % 2 == 0){
            enemies[i].initialize(x, y, 1);
            enemies[i].draw();
          }
          else{
            enemies[i].initialize(x, y, 2);
            enemies[i].draw();
          }
          x += 4;
        }

        y = 4;
        x = 0;

        for(int i = 0; i < NUM_ENEMIES / 2; i++){
          if(i % 2 == 0){
            enemies[i].initialize(x, y, 2);
            enemies[i].draw();
          }
          else{
            enemies[i].initialize(x, y, 1);
            enemies[i].draw();
          }
          x += 4;
        }
      }

    if(level == 3){
      int count = 1;
      for(int i = 0; i < NUM_ENEMIES / 2; i++){
          if(i % 2 == 0){
            enemies[i].initialize(x, y, count);
            enemies[i].draw();
          }
          else{
            enemies[i].initialize(x, y, count);
            enemies[i].draw();
          }
          x += 4;

          count++;

          if (count == 6) {
            count = 1;
          }
        }

        y = 4;
        x = 0;

        for(int i = 0; i < NUM_ENEMIES / 2; i++){
          if(i % 2 == 0){
            enemies[i].initialize(x, y, count);
            enemies[i].draw();
          }
          else{
            enemies[i].initialize(x, y, count);
            enemies[i].draw();
          }
          x += 4;
          count++;

          if (count == 6) {
            count = 1;
          }
        }
    }

    if(level == 4){
      for(int i = 0; i < NUM_ENEMIES / 2; i++){
        if(i % 2 == 0){
          enemies[i].initialize(x, y, 5);
          enemies[i].draw();
        }
        else{
          enemies[i].initialize(x, y, 4);
          enemies[i].draw();
        }
        x += 4;
      }

      y = 4;
      x = 0;
      for(int i = 0; i < NUM_ENEMIES / 2; i++){
        if(i % 2 == 0){
          enemies[i].initialize(x, y, 2);
          enemies[i].draw();
        }
        else{
          enemies[i].initialize(x, y, 3);
          enemies[i].draw();
        }
        x += 4;
      }
    }

    if(level > 4){
      for(int i = 0; i < NUM_ENEMIES; i++) {
        int j = random(1, 8);
        enemies[i].initialize(x, y, j);
        enemies[i].draw();

        x += 4;

        if (i == 7) {
          x = 0;
          y = 4;
        }
      }
    }

    player.draw();

    Serial.println("Finished setupGame()");

  }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix

    //this is the main function
    void update(int potentiometer_value, bool button_pressed) {
      //this below is the actual operation

      int time = 0;
      if (millis() - time > MIN_MOVE_ENEMY) {
        for (int i = 0; i < NUM_ENEMIES; i++) {
          enemies[i].move();
          enemies[i].erase();
          enemies[i].draw();
        }
        time = millis();
      }

      delay(2000);
      int potentiometer_value1 = analogRead(POTENTIOMETER_PIN_NUMBER);

      if (abs(potentiometer_value - potentiometer_value1) > MIN_POTENT_DIFF) {
        player.erase();
        player.set_x(player.get_x() - 1);
        player.draw();
      }
      
      

      if (level_cleared()) {
        level++;
        setupGame();
      }
    }

  private:
    int level;
    unsigned long time;
    const int POTENT_DIFF = 40;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      int count = 0;
      for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].get_strength() == 0) {
          count++;
        }
      }
      if (count == NUM_ENEMIES) {
        return true;
      }
      else {
        return false;
      }
    }

    // set up a level
    void reset_level() {
      //not sure if this is right
      player.die();
      setupGame();
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setupGame();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.update(potentiometer_value, button_pressed);
}

// displays Level
void print_level(int level) {
  matrix.print("Level ");
  matrix.print(level);

  delay(3000);
  matrix.fillScreen(BLACK.to_333());
}

// displays number of lives
void print_lives(int lives) {
  matrix.fillScreen(BLACK.to_333());

  matrix.setCursor(0, 0);
  matrix.print("Lives  ");
  matrix.print(lives);

  delay(3500);
  matrix.fillScreen(BLACK.to_333());
}

// displays "game over"
void game_over() {
  //****** messing again - same setting as print_level 
  matrix.setCursor(8, 10);
  matrix.setTextSize(5);

  matrix.print("Game Over");
}
