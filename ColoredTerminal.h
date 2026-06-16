#pragma once
#define COLORED_TERMINAL_H

#include <iostream>
#include <string>

using namespace std;

//  ANSI Color Codes


// Text colors
#define CLR_RESET   "\033[0m"
#define CLR_BOLD    "\033[1m"

// Foreground colors
#define FG_BLACK    "\033[30m"
#define FG_RED      "\033[31m"
#define FG_GREEN    "\033[32m"
#define FG_YELLOW   "\033[33m"
#define FG_BLUE     "\033[34m"
#define FG_MAGENTA  "\033[35m"
#define FG_CYAN     "\033[36m"
#define FG_WHITE    "\033[37m"

// Bright foreground
#define FG_BRED     "\033[91m"
#define FG_BGREEN   "\033[92m"
#define FG_BYELLOW  "\033[93m"
#define FG_BBLUE    "\033[94m"
#define FG_BMAGENTA "\033[95m"
#define FG_BCYAN    "\033[96m"
#define FG_BWHITE   "\033[97m"

// Background colors
#define BG_BLUE     "\033[44m"
#define BG_CYAN     "\033[46m"
#define BG_BLACK    "\033[40m"
#define BG_DARK     "\033[100m"


void printLine(int width = 52) {
    cout << FG_CYAN "+" CLR_RESET;
    for (int i = 0; i < width; i++) cout << FG_CYAN "-" CLR_RESET;
    cout << FG_CYAN "+" CLR_RESET "\n";
}

void printDoubleLine(int width = 52) {
    cout << FG_BBLUE "+" CLR_RESET;
    for (int i = 0; i < width; i++) cout << FG_BBLUE "=" CLR_RESET;
    cout << FG_BBLUE "+" CLR_RESET "\n";
}

// Main section banner  bold blue background
void printTitle(string title, int width = 52) {
    printDoubleLine(width);
    int pad = (width - (int)title.size()) / 2;
    int rpad = width - pad - (int)title.size();
    cout << FG_BBLUE "| " CLR_RESET;
    for (int i = 0; i < pad - 2; i++) cout << " ";
    cout << CLR_BOLD FG_BYELLOW << title << CLR_RESET;
    for (int i = 0; i < rpad - 2; i++) cout << " ";
    cout << FG_BBLUE " |" CLR_RESET "\n";
    printDoubleLine(width);
}

// Sub-section header - cyan
void printSubTitle(string title, int width = 52) {
    printLine(width);
    int pad = (width - (int)title.size()) / 2;
    int rpad = width - pad - (int)title.size();
    cout << FG_CYAN "| " CLR_RESET;
    for (int i = 0; i < pad - 2; i++) cout << " ";
    cout << CLR_BOLD FG_BCYAN << title << CLR_RESET;
    for (int i = 0; i < rpad - 2; i++) cout << " ";
    cout << FG_CYAN " |" CLR_RESET "\n";
    printLine(width);
}

// Key-value row
void printRow(string label, string value, int width = 52) {
    string entry = "  " + label + ": " + value;
    cout << FG_CYAN "| " CLR_RESET;
    cout << FG_BWHITE << label << CLR_RESET << ": ";
    cout << FG_BGREEN << value << CLR_RESET;
    int used = 2 + (int)label.size() + 2 + (int)value.size();
    for (int i = used; i < width - 1; i++) cout << " ";
    cout << FG_CYAN "|" CLR_RESET "\n";
}

// Single text row
void printRow(string entry, int width = 52) {
    cout << FG_CYAN "| " CLR_RESET;
    cout << FG_BWHITE << entry << CLR_RESET;
    int used = 2 + (int)entry.size();
    for (int i = used; i < width - 1; i++) cout << " ";
    cout << FG_CYAN "|" CLR_RESET "\n";
}

// Success message
void printSuccess(string msg) {
    cout << FG_BGREEN "  [OK] " CLR_RESET << FG_BWHITE << msg << CLR_RESET "\n";
}

// Error / warning message
void printError(string msg) {
    cout << FG_BRED "  [!!] " CLR_RESET << FG_BYELLOW << msg << CLR_RESET "\n";
}

// Info message
void printInfo(string msg) {
    cout << FG_BCYAN "  [>>] " CLR_RESET << FG_WHITE << msg << CLR_RESET "\n";
}

// Undo message
void printUndo(string msg) {
    cout << FG_BMAGENTA "  [<-] " CLR_RESET << FG_BWHITE << msg << CLR_RESET "\n";
}

// Redo message
void printRedo(string msg) {
    cout << FG_BYELLOW "  [->) " CLR_RESET << FG_BWHITE << msg << CLR_RESET "\n";
}

// Traversal arrow  (coach chain etc)
void printTraversal(string line) {
    cout << "  " FG_BBLUE "Engine " CLR_RESET;
    cout << FG_BYELLOW << line << CLR_RESET;
    cout << FG_BBLUE " Caboose" CLR_RESET "\n";
}
