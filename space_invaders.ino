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
const uint8_t POTENTIOMETER_PIN_NUMBER = 4;
const uint8_t BUTTON_PIN_NUMBER = 11;

const uint8_t POTENTIOMETER_PIN_NUMBER2 = 5;
const uint8_t BUTTON_PIN_NUMBER2 = 10;

const uint8_t numReadings = 10;

int readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
int total = 0;              // the running total
int average = 0;            // the average

int readings2[numReadings];  // the readings from the analog input
int readIndex2 = 0;          // the index of the current reading
int total2 = 0;              // the running total
int average2 = 0;            // the average

// global constant for the number of Invaders in the game
const uint8_t NUM_ENEMIES = 14;

// globla constant for the width/height of LED screen
//*********************** was this added later by choyce?
const uint8_t LED_HEIGHT = 16; 
const uint8_t LED_WIDTH = 32; 

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

    // new stuff delete !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        void move_left() {
      x--;
    }
        void move_right() {
      x++;
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
    Player(int num) {
      x = 15;
      y = 14;
      lives = 3;
      n = num;
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
      if (l < 5 && l > 0) {
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
      if (n == 1) {
        draw_with_rgb(AQUA);
      }
      else {
        draw_with_rgb(LIME);
      }
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
    int n;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    //************** changed a bit - cannot use same format as previous draw_with_rgb because need to create a new hardcoded STENCIL for player (seen above)
    void draw_with_rgb(Color color) {
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

      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].initialize(2, 0, 0);
      }

      int x = 2;
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
        x = 2;

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
          x = 2;
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
      x = 2;
      
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
        if (cleared) {
          strengths[i] = random(1, 8);
          
        }
      }
      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].initialize(x, y, strengths[i]);
        enemies[i].draw();

        x += 4;

        if (i == 6) {
          x = 2;
          y = 4;
        }
      }
    }

    player.draw();
    if (multiplayer == true) {
      player2.draw();
    }
  }
    
  // advances the game simulation one step and renders the graphics
  // see spec for details of game
  // Modifies: global variable matrix
  void update(int potentiometer_value, bool button_pressed, bool button_pressed2) {

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

          // checking if min time to move cannonball
          if (millis() - time2 > MIN_MOVE_BALL) {
            ball.erase();
            ball.move();
            ball.draw();
            time2 = millis();
          }
        }
      }
    }

    // checking if min time to move enemies
    if (millis() - time1 > MIN_MOVE_ENEMY && level != 0) {
      int count = 0;
      for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
        if (enemies[i].get_strength() == 0) {
          count++;
        }
      }

      if (level == 1 && (enemies[0].get_y() == 0 && enemies[0].get_x() == 2 || enemies[0].get_y() == 0 && enemies[0].get_x() == 1)) {
          for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
            enemies[i].erase();
            enemies[i].move_left();
            enemies[i].draw();
          }
          v++;
          if (v = 2) {
            movedown = false;
            moveright = false;
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
      else if (level > 1 && (enemies[7].get_y() == 4 && enemies[7].get_x() == 2 || enemies[7].get_y() == 4 && enemies[7].get_x() == 1)) {
          for (int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; i++) {
            enemies[i].erase();
            enemies[i].move_left();
            enemies[i].draw();
          }
          v++;
          if (v = 2) {
            movedown = false;
            moveright = false;
          }

      }
      
      else if (level > 1 && count == NUM_ENEMIES / 2 && (enemies[0].get_y() == 0 && enemies[0].get_x() == 2 || enemies[0].get_y() == 0 && enemies[0].get_x() == 1)) {
        for (int i = 0; i < NUM_ENEMIES / 2; i++) {
          enemies[i].erase();
          enemies[i].move_left();
          enemies[i].draw();
        }
        v++;
        if (v = 2) {
          movedown = false;
          moveright = false;
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
        for (int j = 0; j < 4; j++) {
          for (int i = NUM_ENEMIES - 1; i >= NUM_ENEMIES / 2; i--) {
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
            for (int i = NUM_ENEMIES / 2 - 1; i >= 0; i--) {
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
        for (int j = 0; j < 4; j++) {
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

    // dealing with player2
    if (multiplayer == true) {
      if (button_pressed2) {
        if (!(ball2.has_been_fired())) {
          ball2.reset();
          ball2.fire(player2.get_x() + 1, player2.get_y() - 1);
          ball2.draw();
        }
      }

      // checking for collisions between invaders and cannonball
      for (int i = 0; i < NUM_ENEMIES; i++) {
        if (ball2.has_been_fired()) {
          if ((enemies[i].get_y() + 2 == ball2.get_y() - 1 && enemies[i].get_x() == ball2.get_x()) || (enemies[i].get_y() + 2 == ball2.get_y() - 1 && enemies[i].get_x() + 3 == ball2.get_x())) {
            if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
            }
          }
          else if ((enemies[i].get_y() + 2 == ball2.get_y() && enemies[i].get_x() + 1 == ball2.get_x()) || (enemies[i].get_y() + 2 == ball2.get_y() && enemies[i].get_x() + 2 == ball2.get_x())) {
            if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
            }
          }
          else if ((enemies[i].get_y() == ball2.get_y() - 1 && enemies[i].get_x() + 1 == ball2.get_x()) || (enemies[i].get_y() == ball2.get_y() - 1 && enemies[i].get_x() + 2 == ball2.get_x())) {
            if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
            }
          }
          else if ((enemies[i].get_y() + 2 == ball2.get_y() - 1 && enemies[i].get_x() == ball2.get_x()) || (enemies[i].get_y() == ball2.get_y() - 1 && enemies[i].get_x() + 3 == ball2.get_x())) {
            if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
            }
          }
          else if ((enemies[i].get_y() + 1 == ball2.get_y()) || (enemies[i].get_y() + 1 == ball2.get_y() + 1)) {
            if (enemies[i].get_x() == ball2.get_x()) {
              if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
              }
            }
            else if (enemies[i].get_x() + 1 == ball2.get_x()) {
              if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
              }
            }
            else if (enemies[i].get_x() + 2 == ball2.get_x()) {
              if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
              }
            }
            else if (enemies[i].get_x() + 3 == ball2.get_x()) {
              if (enemies[i].get_strength() > 0) {
              ball2.hit();
              enemies[i].hit();
              break;
              }
            }
          }

          // checking if min time to move cannonball
          if (millis() - time22 > MIN_MOVE_BALL) {
            ball2.erase();
            ball2.move();
            ball2.draw();
            time22 = millis();
          }
        }
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

        // checking if min time to move cannonball
        if (millis() - time2 > MIN_MOVE_BALL) {
          ball.erase();
          ball.move();
          ball.draw();
          time2 = millis();
        }
      }

      // checking if enemies have reached end of screen
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
          Serial.println(average);

          player.erase();
          player.set_x(average / 32 - 1);
          player.draw();

          if (multiplayer == true) {
            // smoothing potentiometer value
            total2 = total2 - readings2[readIndex2];
            // read from the sensor:
            readings2[readIndex2] = analogRead(POTENTIOMETER_PIN_NUMBER2);
            // add the reading to the total2:
            total2 = total2 + readings2[readIndex2];
            // advance to the next position in the array:
            readIndex2 = readIndex2 + 1;

            // if we're at the end of the array...
            if (readIndex2 >= numReadings) {
              // ...wrap around to the beginning:
              readIndex2 = 0;
            }

            // calculate the average:
            average = total2 / numReadings;
            
            player2.erase();
            player2.set_x(average / 32 - 1);
            player2.draw();
          }
      }
    }

    if (level_cleared()) {
      cleared = true;
      level++;
      setupGame();
    }

    // Serial.println(analogRead(POTENTIOMETER_PIN_NUMBER));
    // Serial.println(player.get_x());
    // Serial.println(player.get_y());
    // Serial.println(average);
  }

  private:
    uint8_t level;

    uint8_t v = 0;
    uint8_t j = 0;
    uint8_t q = 0;
    bool movedown = false;
    bool moveright = false;

    bool multiplayer = false;

    unsigned long time1 = millis();
    unsigned long time2 = millis();
    unsigned long time22 = millis();
    bool cleared = true;
    const int MIN_MOVE_ENEMY = 1500;
    const uint8_t MIN_MOVE_BALL = 20;
    const int MIN_MOVE_PLAYER = 1000;
    const uint8_t MIN_POTENT_DIFF = 20;
    Player player = Player(1);
    Player player2 = Player(2);
    Cannonball ball;
    Cannonball ball2;
    int strengths[NUM_ENEMIES] = {};
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
      cleared = false;
      player.die();
      
      if (player.get_lives() < 1) {
        level = 1;
      }
      
      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].initialize(2, 0, 0);
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
  randomSeed(analogRead(6));
  game.setupGame();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  bool button_pressed2 = (digitalRead(BUTTON_PIN_NUMBER2) == HIGH);

  game.update(potentiometer_value, button_pressed, button_pressed2);
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
