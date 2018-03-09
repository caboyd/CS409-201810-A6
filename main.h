#pragma once

//Main functions
void init();
void update();
void reshape(int w, int h);
void display();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);

Game game;





