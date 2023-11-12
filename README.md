# eecs183-final-project

Double check that everything is wired correctly. There should be two (2) wires between the Arduino and the breadboard, four (4) wires between the breadboard and the LED screen, and twelve (12) wires between the Arduino and the LED screen. (You will have more if you’ve wired the potentiometer and/or the button.)

## Todo
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

if (time > MIN_MOVE_ENEMY) {
  moveEnemyRowDown(enemy[i]);
  time = millis();
}

**Update**
Given this description, the key part of your code will need to do the following:
- Update the position of the player based on the value of the potentiometer
- Update the position of the cannonball, including detecting if a new cannonball is being fired
- Update the position of each invader
- Detect if the cannonball is colliding with any of the invaders; if so, both the cannonball and the invader disappear
- Detect if the player is colliding with any of the invaders; if so, the player loses a life and the level restarts
- Detect if all the invaders are killed; if so, start the next level

//go back to if statement again
```



