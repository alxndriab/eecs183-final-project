# eecs183-final-project
# Reach
> Need to get 50 points

todo 
- [ ] change random seed to digital pin

### 20p - invaders move side to side
invaders cannot move off screen so check if need exactly 16 invaders

### 20p - add player 2
wire a second potentiometer and button

bool multiplayer = false #change depending on start menu
```
public:
setupGame() {
  if (multiplayer) {
    player2.draw();
  }
}

update() {
  if (ball2.has_been_fired) {
   ... ball collisions
  }
}

// place to draw ball2
private:
  Player player2;
  Cannonball ball2;
```
# Teresa Multiplayer
I copied and pasted the player class and the cannonball class, renaming them Player_1 and Player_2 and Cannonball_1 and Cannonball_2
  Player_1 is aqua and has an orange cannonball (Cannonball_1), and Player_2 is lime and has a yellow cannonball (Cannonball_2) to differentiate. 
  As of right now, I have only Player_1 as having lives and being able to die because I wasn't sure how lives would work with two players and figured they could just share lives.
    However, if there's an invader touching player 2 and not player 1, you will not die until the invader touches the bottom of the screen, which could be a problem

As for the rest of the code, I have been copying and pasting everything that has an instance of player or ball and changing the names accordingly. 
I haven't quite finished the potentiometer and button stuff because I didn't know their numbers
I have a couple of if statements for muliplayer, but the menu isn't implemented yet, so there is nothing to actually set the mutiplayer to true or false
I also added more parameters to UPDATE; I added potentiometer and button 2 along with the multiplayer bool



### 10p - add title screen with options
Maybe have the screen when level = 0 and like stop game make sure the game DOESN'T start

# Core
https://docs.arduino.cc/built-in-examples/analog/Smoothing
Double check that everything is wired correctly. There should be two (2) wires between the Arduino and the breadboard, four (4) wires between the breadboard and the LED screen, and twelve (12) wires between the Arduino and the LED screen. (You will have more if you’ve wired the potentiometer and/or the button.)

## Todo
11/13
- collisions of cannonball and space invaders
- how to handle when player.get_lives() = 0
1. print game over
2. send you back to level 1 and reset lives to 3
- time and moving player with potentiometer and space invaders 

11/12
- collisions and decreasing the invaders strength
- cannonball...
- lives and reset_level() and level_cleared()
- time and moving player with potentiometer and space invaders 

# SLIDES
https://docs.google.com/presentation/d/1lle_tu7X_vg936_-UBO_V36wvrSDhdXmX1PCVqaTTHo/edit#slide=id.g294a140e427_0_358

# Debugging
`Serail.println()` to print to console
Test with kit often:
- one row of enemies at a time
- order of operations count
- one ball at a time

```arduino
// as you move the potentiometer,
// this value should change
Serial.println(potentiometer_value);

// should print 1 when button is pressed
// should print 0 when button is not pressed
Serial.print(button_pressed);

// LED
matrix.print("Level: ");
matrix.print(level);
matrix.fillScreen(BLACK.to_333());


```

# Workflow
**Timeline**
Player -> Invaders -> Ball -> Interactions -> Levels 2-5

## invader and Cannonball
Draw the enemies, set up the first level
Move the top row of enemies down the display
Introduce the ball.
Fire the ball
Move ball up the display
Stop ball when it reaches the top


### Moving an Object on the Display
To move an object, i.e., Player, Invader, or Cannonball, you must 
1. manually redraw all of the pixels
2. erase() the pixels of the instance
3. Update the position (x or y) of the instance
4. draw() the pixels of the instance
  The erase() and draw() functions will call draw_with_rgb()

## Interactions and Levels
1. Detect collisions
- Between ball and enemies
- Between enemies and player
- Between enemies and bottom of display
2. Create level 2
3. Develop transitions between levels, game over, restarting a level
4. Create remaining levels

## Game - Time and Looping
Keep track of the last time you moved the enemies, then keep checking to see if at least some minimum time has passed
1. call millis and store return value
2. If current time is greater than last time enemies were moved by at least 2 seconds
- Move the row of enemies down
- Update the last time invaders were moved down to now
3. Go to step 1
```
unsigned long time = millis();
const int MIN_MOVE_ENEMY = 2000;
const int MIN_MOVE_PLAYER = 3000;
const int MIN_POTENT_DIFF = 40;

update(potentiometer_value, ...) {
  if (millis() - time > MIN_MOVE_ENEMY) {
    moveEnemyRowDown(enemy[i]);
    time = millis();
  }
  
  if (potentiometer_value - GET-CURRENT-POTENTIOMETER-VALUE > MIN_POTENT_DIFF) {
    player.set_x -= 1;
    # need to redraw
    # player.draw();
  }
}
```
**inside update()**
Given this description, the key part of your code will need to do the following:
- Update the position of the player based on the value of the potentiometer
- Update the position of the cannonball, including detecting if a new cannonball is being fired
- Update the position of each invader
- Detect if the cannonball is colliding with any of the invaders; if so, both the cannonball and the invader disappear
- Detect if the player is colliding with any of the invaders; if so, the player loses a life and the level restarts
- Detect if all the invaders are killed; if so, start the next level

//go back to if statement again
