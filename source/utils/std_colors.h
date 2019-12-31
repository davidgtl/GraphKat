//
// Created by david on 01/01/2020.
//

#ifndef GRAPHKAT_STD_COLORS_H
#define GRAPHKAT_STD_COLORS_H

/* FOREGROUND */
// These codes set the actual text to the specified color
#define RST  "\x1B[0m" // Set all colors back to normal.
#define FOREBLK  string("\x1B[30m") // Black
#define FORERED  string("\x1B[31m") // Red
#define FOREGRN  string("\x1B[32m") // Green
#define FOREYEL  string("\x1B[33m") // Yellow
#define FOREBLU  string("\x1B[34m") // Blue
#define FOREMAG  string("\x1B[35m") // Magenta
#define FORECYN  string("\x1B[36m") // Cyan
#define FOREWHT  string("\x1B[37m") // White

/* BACKGROUND */
// These codes set the background color behind the text.
#define BACKBLK string("\x1B[40m")
#define BACKRED string("\x1B[41m")
#define BACKGRN string("\x1B[42m")
#define BACKYEL string("\x1B[43m")
#define BACKBLU string("\x1B[44m")
#define BACKMAG string("\x1B[45m")
#define BACKCYN string("\x1B[46m")
#define BACKWHT string("\x1B[47m")

#endif //GRAPHKAT_STD_COLORS_H
