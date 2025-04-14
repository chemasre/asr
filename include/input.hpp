#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include <common_header.hpp>
#include <common_system_header.hpp>

#define KEY_ESC 27
#define KEY_SPACE ' '
#define KEY_1 '1'
#define KEY_2 '2'
#define KEY_3 '3'
#define KEY_4 '4'
#define KEY_5 '5'
#define KEY_6 '6'
#define KEY_7 '7'
#define KEY_8 '8'
#define KEY_9 '9'
#define KEY_0 '0'
#define KEY_PLUS VK_OEM_PLUS
#define KEY_MINUS VK_OEM_MINUS
#define KEY_F1 VK_F1
#define KEY_F2 VK_F2
#define KEY_F3 VK_F3
#define KEY_F4 VK_F4
#define KEY_F5 VK_F5
#define KEY_F6 VK_F6
#define KEY_F7 VK_F7
#define KEY_F8 VK_F8
#define KEY_F9 VK_F9
#define KEY_F10 VK_F10
#define KEY_F11 VK_F11
#define KEY_F12 VK_F12

#define MOUSE_LEFT VK_LBUTTON
#define MOUSE_RIGHT VK_RBUTTON


int isKeyPressed(int k);
int isKeyDown(int k);

void initInput();
void getMousePosition(int* x, int* y);

#endif
