#!/bin/bash
clear
c++ -O2 -o imgGenerator 1.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
