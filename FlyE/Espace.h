/*
 * Espace class for FlyE
 * 
 * An Espace is essentially a container for the grid of e-field values in the accelerator. However, this grid is stored contiguously in field_ and Espace
 * provides pseudomultidimensional access with at(), normAt() and magAt(). The e-field is treated as 4-dimensional: 3 dimensions for the position in space,
 * and the final dimension as the direction of the field at the given point. The normField_ is maintained as the normalised field so we can safely scale field_.
 * 
 * Espaces *must* be constructed with the params struct, which is defined here and contains various properties of the environment.
 * 
 * The additional abilities of Espace are as follows:]
 * 	gradientX(x,y,z), gradientY(x,y,z), gradientZ(x,y,z) - these functions get the field gradient at a given point.
 * 	magAt(x,y,z) - this gets the field magnitude at a given point.
 * 	normAt(x,y,z,d) accesses normField_, at(x,y,z,d) accesses field_
 * 
 * Additionally, the compound addition/assignment operator ( += ) is overloaded for the class to add together e-fields as in matrix/tensor addition. 
 * 
 * Electrode is derived from Espace and offers methods specific to handling electrodes:
 * 
 * import(n) imports the files generated by ExSimEck for electrode number n into normField_ 
 * setVoltage(v) updates field_ with the values of normField_ multiplied by v
 * 
 * Electrode also offers the static method sumElectrodes(allElectrodes, config, nToSum), which takes a vector of (shared_pointers to) Electrodes and sums nToSum of them
 * into an Espace(config). It then returns this Espace with std::move to explicitly avoid a copy. If nToSum is not provided then all Electrodes in the vector are summed together.
 * This summation is achieved using the aforementioned compound assignment overload, which gives good readability but is slow if the compilation is not optimised (if it is optimised
 * then the operation is expanded inline).
 *
 * Jamie Parkinson, July 2014
 */

#pragma once

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <memory>
#include <vector>

#ifndef N_DIMENSIONS
#define N_DIMENSIONS 3
#endif

#ifndef SIMION_CORRECTION
#define SIMION_CORRECTION 0.1
#endif

#ifndef MM_M_CORRECTION
#define MM_M_CORRECTION 1000
#endif

const char dimensions[N_DIMENSIONS] = { 'X', 'Y', 'Z' };

#include "myStructs.h"

class Espace {  // General representation of the pseudo-multidimensional array that we use to represent our space
 protected:
  std::vector<float> field_, normField_;
  params config_;

 public:
  // Constructor
  Espace(const params &config);

  // Get the gradient at point (x,y,z)
  float gradientX(const int x, const int y, const int z);
  float gradientY(const int x, const int y, const int z);
  float gradientZ(const int x, const int y, const int z);

  // Functions to get the field value at (x,y,z,d)
  float& at(const int x, const int y, const int z, const int d);
  float& normAt(const int x, const int y, const int z, const int d);

  // Gets field magnitude at (x,y,z)
  float magAt(const int x, const int y, const int z);

  // Overload the addition operator
  Espace& operator+=(const Espace& b);
};

class Electrode : public Espace {  // Adds voltage and file import functions
 private:
  float voltage_;

 public:
  // Constructor (to which the config struct is passed)
  Electrode(params &config);

  // Constructor to import electrode n
  Electrode(int n, params &config);

  // Import electrode n
  void import(int n);

  // Sets the electrode to voltage v
  void setVoltage(float v);

  // For summing Electrodes
  static Espace sumElectrodes(
      const std::vector<std::shared_ptr<Electrode> > &electrodes,
      params &config, int nToSum = 0);
};
