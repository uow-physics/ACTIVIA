// Define various constants

#include "Activia/ActConstants.hh"

#include <cmath>

const double ActConstants::NA = 6.022e-4; // Avogadro's number times 10^-27 (mb in cm^2)
const double ActConstants::eps = 1.0e-10; // accuracy
const double ActConstants::mElec = 1.6605402e-27; // mass of electron (in kg)
const double ActConstants::qElec = 1.602189e-19; // charge of electron (in Colombs)
const double ActConstants::c = 2.99792458e8; // speed of light (in ms^-1)
const double ActConstants::permittivity = 8.8544e-12; // permittivity of free space
const double ActConstants::pi = acos(-1.0);
const double ActConstants::phi0 = 360.0;
const double ActConstants::degToRad = acos(-1.0)/180.0;
const double ActConstants::hbarc = 197.8;
// Calculate multiplier pps * flux (/cm2sec) * sigma (mb) = N/day
const double ActConstants::pps = ActConstants::NA*24.0*3600.0*1000.0;
const double ActConstants::ln2 = 0.6931;
