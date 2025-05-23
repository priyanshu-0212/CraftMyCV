//========================================================================
//
// SplashScreen.h
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2009, 2018, 2020, 2021 Albert Astals Cid <aacid@kde.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "SplashTypes.h"

#include <cstdlib>

//------------------------------------------------------------------------
// SplashScreen
//------------------------------------------------------------------------

class SplashScreen
{
public:
    explicit SplashScreen(const SplashScreenParams *params);
    explicit SplashScreen(const SplashScreen *screen);
    ~SplashScreen();

    SplashScreen(const SplashScreen &) = delete;
    SplashScreen &operator=(const SplashScreen &) = delete;

    SplashScreen *copy() const { return new SplashScreen(this); }

    // Return the computed pixel value (0=black, 1=white) for the gray
    // level <value> at (<x>, <y>).
    int test(int x, int y, unsigned char value)
    {
        int xx, yy;
        if (mat == nullptr) {
            createMatrix();
        }
        xx = x & sizeM1;
        yy = y & sizeM1;
        return value < mat[(yy << log2Size) + xx] ? 0 : 1;
    }

    // Returns true if value is above the white threshold or below the
    // black threshold, i.e., if the corresponding halftone will be
    // solid white or black.
    bool isStatic(unsigned char value)
    {
        if (mat == nullptr) {
            createMatrix();
        }
        return value < minVal || value >= maxVal;
    }

private:
    void createMatrix();

    void buildDispersedMatrix(int i, int j, int val, int delta, int offset);
    void buildClusteredMatrix();
    int distance(int x0, int y0, int x1, int y1);
    void buildSCDMatrix(int r);

    const SplashScreenParams *screenParams; // params to create the other members
    unsigned char *mat; // threshold matrix
    int size; // size of the threshold matrix
    int sizeM1; // size - 1
    int log2Size; // log2(size)
    unsigned char minVal; // any pixel value below minVal generates
                          //   solid black
    unsigned char maxVal; // any pixel value above maxVal generates
                          //   solid white
};

#endif
