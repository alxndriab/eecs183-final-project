# eecs183-final-project

Double check that everything is wired correctly. There should be two (2) wires between the Arduino and the breadboard, four (4) wires between the breadboard and the LED screen, and twelve (12) wires between the Arduino and the LED screen. (You will have more if you’ve wired the potentiometer and/or the button.)

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

## Interactions
Detect collisions
Between ball and enemies
Between enemies and player
Between enemies and bottom of display
Create level 2
Develop transitions between levels, game over, restarting a level
Create remaining levels

## Game - Time and Looping

