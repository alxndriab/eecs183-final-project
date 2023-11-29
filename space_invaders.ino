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
const int POTENTIOMETER_PIN_NUMBER = 4;
const int BUTTON_PIN_NUMBER = 11;

const int numReadings = 10;

int readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
int total = 0;              // the running total
int average = 0;            // the average

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 14;

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
void print_numPlayer();

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
    void moveright() {
      x++;
    }
    void moveleft() {
      x--;
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
      erase();
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
      fired = true;
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
    void draw() {
      if (has_been_fired()) {
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
      if (x_arg + 2 <= LED_WIDTH && x_arg >= -1) {
        x = x_arg;
      }
    }

    void set_lives(int l) {
      if (l < 4 && l > 0) {
        lives = l;
      }
    }
    
    // Modifies: lives
    void die() {
      if (lives > 0) {
        lives--;
      }
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
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setupGame() {

      // checking if needing to reset level
      if (player.get_lives() == 0) {
        game_over();
        player.set_lives(3);
        level = 1;
        delay(5000);
      }
      
      matrix.fillScreen(BLACK.to_333());
      matrix.setCursor(0,0);

      int x = 0;
      int y = 0;

      if (level != 0) {
        print_level(level);
        print_lives(player.get_lives());
      }

      // settting up level
      if (level == 0) {
        print_numPlayer();
        enemies[0].initialize(4, 8, 5);
        enemies[0].draw();
        enemies[1].initialize(22, 8, 4);
        enemies[1].draw();

        // delay(10000);
        //level = 1;
      }
      else if (level == 1) {
        for (int i = 0; i < NUM_ENEMIES; i++) {
          if (i < NUM_ENEMIES / 2) {
            enemies[i].initialize(x, y, 1);
            enemies[i].draw();
            x += 4;
          }
          else {
            enemies[i].initialize(0, 0, 0);
          }
        }
      }
      else if (level == 2) {
        for (int i = 0; i < NUM_ENEMIES / 2; i++) {
          if (i % 2 == 0) {
            enemies[i].initialize(x, y, 1);
            enemies[i].draw();
          }
          else {
            enemies[i].initialize(x, y, 2);
            enemies[i].draw();
          }
          x += 4;
        }

        y = 4;
        x = 0;

        for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
          if (i % 2 == 0) {
            enemies[i].initialize(x, y, 2);
            enemies[i].draw();
          }
          else {
            enemies[i].initialize(x, y, 1);
            enemies[i].draw();
          }
          x += 4;
        }
      }
      else if (level == 3) {
        int count = 1;
        for (int i = 0; i < NUM_ENEMIES; i++) {
          if (i == NUM_ENEMIES / 2) {
            y = 4;
            x = 0;
          }
          if (i % 2 == 0) {
            enemies[i].initialize(x, y, count);
            enemies[i].draw();
          }
          else {
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
        
      else if (level == 4) {
        for (int i = 0; i < NUM_ENEMIES / 2; i++) {
          if (i % 2 == 0) {
            enemies[i].initialize(x, y, 5);
            enemies[i].draw();
          }
          else {
            enemies[i].initialize(x, y, 4);
            enemies[i].draw();
          }
          x += 4;
        }

        y = 4;
        x = 0;
        
        for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
          if (i % 2 == 0) {
            enemies[i].initialize(x, y, 2);
            enemies[i].draw();
          }
          else {
            enemies[i].initialize(x, y, 3);
            enemies[i].draw();
          }
          x += 4;
        }
      }

      else if (level > 4) {
        for (int i = 0; i < NUM_ENEMIES; i++) {
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
  }
    
  // advances the game simulation one step and renders the graphics
  // see spec for details of game
  // Modifies: global variable matrix
  void update(int potentiometer_value, bool button_pressed) {
    
    if (level == 0) {
      for (int i = 0; i < 2; i++) {
        if (ball.has_been_fired()) {
          if (i == 0) {
            if ((enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() == ball.get_x()) || (enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() + 3 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = false;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
            else if ((enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 1 == ball.get_x()) || (enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 2 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = false;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
            else if ((enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 1 == ball.get_x()) || (enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 2 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = false;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
            else if ((enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() == ball.get_x()) || (enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 3 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = false;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
          }
          else if (i == 1) {
            if ((enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() == ball.get_x()) || (enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() + 3 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = true;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
            else if ((enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 1 == ball.get_x()) || (enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 2 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = true;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
            else if ((enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 1 == ball.get_x()) || (enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 2 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = true;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
            else if ((enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() == ball.get_x()) || (enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 3 == ball.get_x())) {
              if (enemies[i].get_strength() > 0) {
                ball.hit();
                enemies[i].hit();
                multiplayer = true;
                level = 1;
                player.set_lives(4);
                matrix.fillScreen(BLACK.to_333());
                reset_level();
                return;
              }
            }
          }
        }
      }

      return;
    }


    bool movedown = false;
    bool moveright = false;

    // checking if min time to move enemies
    if (millis() - time1 > MIN_MOVE_ENEMY && level != 0) {
      if (((enemies[0].get_y() == 0 && enemies[0].get_x() == 2 || enemies[0].get_y() == 0 && enemies[0].get_x() == 1) && level == 1) || (enemies[0].get_y() == 4 && enemies[0].get_x() == 2 || enemies[0].get_y() == 4 && enemies[0].get_x() == 1)) {
        for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
          enemies[i].erase();
          enemies[i].move_left();
          enemies[i].draw();
        }
        v++;
        if (v = 2) {
          movedown = false;
        }

        int count = 0;
        for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
          if (enemies[i].get_strength() == 0) {
            count++;
          }
        }
        if (count == NUM_ENEMIES / 2) {
          for (int i = 0; i < NUM_ENEMIES / 2; i++) {
          enemies[i].erase();
          enemies[i].move_left();
          enemies[i].draw();
          }
        }
      }

      else if (movedown == false) {
        for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
          enemies[i].erase();
          enemies[i].move();
          enemies[i].draw();
        }
        movedown = true;
        
        int count = 0;
        for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
          if (enemies[i].get_strength() == 0) {
            count++;
          }
        }
        if (count == NUM_ENEMIES / 2) {
          for (int i = 0; i < NUM_ENEMIES / 2; i++) {
          enemies[i].erase();
          enemies[i].move();
          enemies[i].draw();
          }
        }
      }
      else if (moveright == false) {
        for (int i = 0; i < 4; i++) {
          for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
            enemies[i].erase();
            enemies[i].move_right();
            enemies[i].draw();
          }
        
          int count = 0;
          for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
            if (enemies[i].get_strength() == 0) {
              count++;
            }
          }

          if (count == NUM_ENEMIES / 2) {
            for (int i = 0; i < NUM_ENEMIES / 2; i++) {
            enemies[i].erase();
            enemies[i].move_right();
            enemies[i].draw();
            }
          } 
        }
        moveright = true;
        movedown = false;
        
      }
      else if (moveright == true) {
        for (int i = 0; i < 4; i++) {
          for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
            enemies[i].erase();
            enemies[i].move_left();
            enemies[i].draw();
          }
        
          int count = 0;
          for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
            if (enemies[i].get_strength() == 0) {
              count++;
            }
          }
          if (count == NUM_ENEMIES / 2) {
            for (int i = 0; i < NUM_ENEMIES / 2; i++) {
              enemies[i].erase();
              enemies[i].move_left();
              enemies[i].draw();
            }
          }
        }
        moveright = false;
        movedown = false;
        
      }
      v = 0;
      q = 0;
      j = 0;
      time1 = millis();
    }

    // firing cannon
    if (button_pressed) {
      if (!(ball.has_been_fired())) {
        ball.reset();
        ball.fire(player.get_x() + 1, player.get_y() - 1);
        ball.draw();
      }
    }

    // checking for collisions between invaders and cannonball
    for (int i = 0; i < NUM_ENEMIES; i++) {
      if (ball.has_been_fired() && level != 0) {
        if ((enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() == ball.get_x()) || (enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() + 3 == ball.get_x())) {
          if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
          }
        }
        else if ((enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 1 == ball.get_x()) || (enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 2 == ball.get_x())) {
          if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
          }
        }
        else if ((enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 1 == ball.get_x()) || (enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 2 == ball.get_x())) {
          if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
          }
        }
        else if ((enemies[i].get_y() + 2 == ball.get_y() - 1 && enemies[i].get_x() == ball.get_x()) || (enemies[i].get_y() == ball.get_y() - 1 && enemies[i].get_x() + 3 == ball.get_x())) {
          if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
          }
        }
        else if ((enemies[i].get_y() + 1 == ball.get_y()) || (enemies[i].get_y() + 1 == ball.get_y() + 1)) {
          if (enemies[i].get_x() == ball.get_x()) {
            if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
            }
          }
          else if (enemies[i].get_x() + 1 == ball.get_x()) {
            if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
            }
          }
          else if (enemies[i].get_x() + 2 == ball.get_x()) {
            if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
            }
          }
          else if (enemies[i].get_x() + 3 == ball.get_x()) {
            if (enemies[i].get_strength() > 0) {
            ball.hit();
            enemies[i].hit();
            break;
            }
          }
        }
      }
      if (ball.has_been_fired()) {
        // checking if min time to move cannonball
        if (millis() - time2 > MIN_MOVE_BALL) {
          ball.erase();
          ball.move();
          ball.draw();
          time2 = millis();
        }
      }

      // checking if enemies have reached the player
      if ((enemies[i].get_y() + 3 == player.get_y() && enemies[i].get_x() == player.get_x()) || (enemies[i].get_y() + 2 == player.get_y() && enemies[i].get_x() + 2 == player.get_x()) || (enemies[i].get_y() + 3 == player.get_y() && enemies[i].get_x() == player.get_x() + 2) || (enemies[i].get_y() + 2 == player.get_y() && enemies[i].get_x() + 2 == player.get_x() + 2) || (enemies[i].get_y() + 2 == ball.get_y() && enemies[i].get_x() + 1 == player.get_x()) || (enemies[i].get_y() + 2 == player.get_y() && enemies[i].get_x() + 1 == player.get_x())) {
        if (enemies[i].get_strength() > 0) {
          reset_level();
          return;
        }
      }

      if (enemies[i].get_y() + 3 == LED_HEIGHT - 2) {
        if (enemies[i].get_strength() > 0) {
          reset_level();
          break;
        }
      }
    }

    for (int i = 0; i < NUM_ENEMIES; i++) {
      if (enemies[i].get_y() + 3 != LED_HEIGHT - 2) {

          // smoothing potentiometer value
          total = total - readings[readIndex];
          // read from the sensor:
          readings[readIndex] = analogRead(POTENTIOMETER_PIN_NUMBER);
          // add the reading to the total:
          total = total + readings[readIndex];
          // advance to the next position in the array:
          readIndex = readIndex + 1;

          // if we're at the end of the array...
          if (readIndex >= numReadings) {
            // ...wrap around to the beginning:
            readIndex = 0;
          }

          // calculate the average:
          average = total / numReadings;
          // send it to the computer as ASCII digits
          Serial.println(average);

          player.erase();
          player.set_x(average / 32 - 1);
          player.draw();
      }
    }

    if (level_cleared()) {
      level++;
      setupGame();
    }
  }

  private:
    int level = 0;
    int i = 0;
    int j = 0;
    bool multiplayer = false;
    unsigned long time1 = millis();
    unsigned long time2 = millis();
    //changed from 6000 to 4000
    const int MIN_MOVE_ENEMY = 2500;
    //change from 30 to 20
    const int MIN_MOVE_BALL = 20;
    const int MIN_MOVE_PLAYER = 1000;
    const int MIN_POTENT_DIFF = 20;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES] = {};

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
      player.die();
      if (player.get_lives() < 1) {
        level = 1;
      }
      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].initialize(0, 0, 0);
      }
      ball.reset();
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
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  randomSeed(digitalRead(13));

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
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(2, 0);
  matrix.print("Level  ");
  matrix.print(level);

  delay(3000);
  matrix.fillScreen(BLACK.to_333());
}

// displays number of lives
void print_lives(int lives) {
  matrix.fillScreen(BLACK.to_333());

  matrix.setCursor(2, 0);
  matrix.print("Lives  ");
  matrix.print(lives);

  delay(3000);
  matrix.fillScreen(BLACK.to_333());
}

void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(5,0);
  matrix.print("Game Over");
}

void print_numPlayer() {
  matrix.setCursor(1, 0);
  matrix.print("1P 2P");
}
