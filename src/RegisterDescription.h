#ifndef REGISTER_DESCRIPTION_H
#define REGISTER_DESCRIPTION_H

struct RegisterDescription {
  const int type;
  const unsigned int registerNumber;
  const bool requiresConversion;
  const float multiplicationFactor;
};

#endif