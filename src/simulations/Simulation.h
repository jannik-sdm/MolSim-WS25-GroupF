//
// Created by jannik on 11/6/25.
//

#pragma once

class Simulation {
public:
  // desctructor to avoid memory leaks
  virtual ~Simulation() = default;

  virtual void iteration() = 0;

};

