//
// Created by jannik on 11/6/25.
//

#ifndef MOLSIM_WS25_GROUPF_SIMULATION_H
#define MOLSIM_WS25_GROUPF_SIMULATION_H

class Simulation {
public:
  // desctructor to avoid memory leaks
  virtual ~Simulation() = default;

  virtual void iteration() = 0;

};

#endif  // MOLSIM_WS25_GROUPF_SIMULATION_H
