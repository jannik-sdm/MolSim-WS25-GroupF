//
// Created by jv_fedora on 12.01.26.
//

#include "MembraneSimulation.h"

void MembraneSimulation::updateF(){
  //Kann ich direkt updateF vom Parent aufrufen?
  //linkedCells.applyToParticles([this](Particle &p) { p.setF({0, g_grav * p.getM(), 0}); });
  linkedCells.applyToParticles([this](Particle &p1) {
    //Zuerst alle Particle Kräfte wieder 0en
      //Soll die Gravity in 3D immer in Z-Richtung verlaufen?
        double current_time = start_time+delta_t*current_iteration;
        //p1.getType() == 2 steht für einer der Partikel, die angehoben werden sollen.
        if (current_time < 150
          //Schau nach, ob auf das Particle F_zUP wirken soll
          && (std::find(this->upwardsParticles.begin()
            , this->upwardsParticles.end()
            , &p1) != this->upwardsParticles.end())) //Vergleiche auf Pointer Gleichheit
          p1.setF({0,0,g_grav*p1.getM()+F_zUp});
        else p1.setF({0, 0, g_grav * p1.getM()});

    //Kraft zwischen Nachbarn
    for (int i = 0; i < 8; i++) {
      // Ohne N3 Optimierung, um einen Iterationsdurchlauf zu sparen
      Particle *p2 = p1.getNeighbor(i);
      if (p2 == nullptr) continue;
      //Diagonal Neighbors
      if (i == 0 || i == 2 || i == 5 || i == 7) Physics::harmonicPotential::forceDiagonal(p1, *p2, this->stiffnessConstant, r0);
      //Direct Neighbors
      else Physics::harmonicPotential::forceStraight(p1, *p2, this->stiffnessConstant, r0);
  }
  });
  //Reguläre Lennard-Jones Force -> Kleinerer Cutoff Radius wird dem Konstruktor übergeben
  linkedCells.applyToPairs([this](Particle &p1, Particle &p2) {
    interactionParams params = mixing_table[p1.getType() * num_types + p2.getType()];
    //Wie sorge ich dafür, dass der cutoff radius kleiner gewählt wird? -> Kann ich diesen hier als Argument mitgeben?
    Vector3 f = Physics::LennardJones::fastForce(p1, p2, params.sigma2, params.epsilon24);
    p1.addF(f);
    p2.subF(f);
  });

}