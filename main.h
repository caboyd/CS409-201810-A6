#pragma once

//Main functions

//Initialize the data for the game.
//Loads assets and builds world, movement graph and pickup manager
void init();

//Calls update on the game class when enough time has passed for fixed delta time
void update();

//GLUT calls this to Resize the window
void reshape(int w, int h);

//Calls render on the game class
void display();

//GLUT callbacks for keyboard/mouse input
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);

//The overarching game class
Game game;





