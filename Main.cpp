/*
FINAL
main.cpp
Created by Ryder Babik
11/11/20
Engineering Computation 24780-B
main file for running the program
*/


#include "fssimplewindow.h"
#include "ViewManager2.h"

int main(void) {
	
	FsOpenWindow(16, 16, 800, 600, 1, "MAZE MANIA - The Maze Runners");
	ViewManager theManager;
	theManager.manage();
}