/* Fm(z;t,tau)
input: 
z is not required, since all z will be tracked and calculated. from 
tau: the frame that choosen to be reference frame.
t will be counted only larger than tau

by my understanding, the waiting time tau_w is just choosing a frame in history as reference.
from that time on, the displacement will be conuted. 

output:
the output will be a table with :
t   z: 1  3  5  7  9  
0    1000 1000 1000 1000
1   ... ... ... ...
2   ... ... ... ...
3   ... ... ... ...

the first line will lack one element so that a # will indicate this

since the system is symmertric in z direction, the z should be the distance from the middle of the system

globally will hold a large vector, index indicate particle (id-1).
Of course xu yu zu will be used

if particle move out of tau_w location in horizontal direction, the particle will not be count again, but not in z direction.

*/

#include <omp.h>

#include <cmath>
#include <iostream>
#include <string>

#include "Base.hpp"
#include "Table.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 6) && (argc != 4) && (argc != 5)) {
    std::cout << "input number should be 3 or 5" << std::endl;
    std::cout << "Fmzt dataFolder tau_w Fmzt_name a height, tau need to be a integer frame\n"
              << "by default a=1.6 and height=2 as in paper, height is set as an integer\n"
              << "Fmzt ./T0004_05 0 Fmzt_tau_0.xyz 1.6 2 \n"
              << "Fmzt ./T0004_05 0 Fmzt_tau_0.xyz 1.6  \n"
              << "Fmzt ./T0004_05 0 Fmzt_tau_0.xyz \n"
              << std::endl;
    return 0;
  }

  std::string folder = argv[1];
  fs::path dirName = fs::u8path(folder);
  int Tau_w = std::stoi(std::string(argv[2]));
  std::string pattern = argv[3];

  if (!fs::is_directory(dirName)) {
    std::cout << "input folder name is invalid!" << std::endl;
    return 0;
  }
  Data data(folder);
  // Data dataTmp(folder);

  double a = 1.6;
  int height = 2;

  if (argc == 6) {
    a = std::stod(argv[4]);
    height = std::stoi(argv[5]);
  } else if (argc == 5) {
    a = std::stod(argv[4]);
  }
  double asq = a * a;

  // the high will not be included
  // load 1 file to get const info
  size_t start = Tau_w, end = data.fileIndex.size();
  // load all the file needed
  // data.load(1, start, start + 1);
  data.load(1, start, end);

  // setup the output z index

  std::cout << "load finished, start setup slab" << std::endl;
  double ztmp = data.front()->boxZL;
  double zmid = data.front()->boxZL + data.front()->zl / 2.0;

  std::vector<int> particleSlab, particleSlabIndex;
  particleSlab.resize(data.front()->particleN);
  particleSlabIndex.resize(data.front()->particleN);
  int maxZ = -200, minZ = 200;
  double halfHeight = height / 2.0;
  // all particle
  for (size_t i = 0; i < particleSlab.size(); i++) {
    // the function can be viewed:
    // https://www.wolframalpha.com/input/?i=ceil%28%28x-3%2F2%29%2F3%29*3
    // the distance from middle is stored
    particleSlab[i] = std::ceil(((data.front()->z[i] - zmid) - halfHeight) / double(height)) * height;
    // std::cout << particleSlab[i] << std::endl;
    if (particleSlab[i] > maxZ) {
      maxZ = particleSlab[i];
    }
    if (particleSlab[i] < minZ) {
      minZ = particleSlab[i];
    }
  }
  std::cout << "slab finished, start setup slab index" << std::endl;

  std::vector<int> ZIndex;
  int zztmp = minZ;
  while (zztmp <= maxZ) {
    ZIndex.push_back(zztmp);
    zztmp += height;
  }

  for (size_t i = 0; i < particleSlab.size(); i++) {
    // the index which the particle should fill in
    particleSlabIndex[i] = (particleSlab[i] - minZ) / height;
  }

  std::cout << "setup slab finished, start setup table" << std::endl;
  // set up the data container
  Table out;
  out.setcols(ZIndex.size() + 1);
  out.setrows(end - start + 1);

  for (size_t i = 1; i < ZIndex.size(); i++) {
    out[i][0] = ZIndex[i];
  }
  for (size_t i = 1; i < end - start; i++) {
    out[0][i] = i;
  }

  std::cout << "setup finished, start cal" << std::endl;
  // start calculation

  Frame* constFrame = data.front();

  double xsq, ysq, xtmp, ytmp;

  for (size_t iFrame = start + 1; iFrame < end; iFrame++) {
    // std::cout << "cal "<<iFrame << std::endl;
    // dataTmp.clear();
    // dataTmp.load(1, iFrame, iFrame + 1);

    for (size_t jParticle = 0; jParticle < particleSlab.size(); jParticle++) {
      if (particleSlab[jParticle] != 10000) {
        // xtmp = dataTmp.front()->x[jParticle] - constFrame->x[jParticle];
        // ytmp = dataTmp.front()->y[jParticle] - constFrame->y[jParticle];
        // ztmp = dataTmp.front()->z[jParticle];
        xtmp = data[iFrame]->x[jParticle] - constFrame->x[jParticle];
        ytmp = data[iFrame]->y[jParticle] - constFrame->y[jParticle];
        ztmp = data[iFrame]->z[jParticle];
        xsq = xtmp * xtmp;
        ysq = ytmp * ytmp;
        // if horizontal out of bound
        // std::cout << xsq << " and " << ysq << std::endl;
        // std::cout << ztmp << " and " << (particleSlab[jParticle] + halfHeight) << std::endl;
        if (xsq + ysq > asq) {
          // set a large number so that particle won't count again
          particleSlab[jParticle] = 10000;
        } else if ((ztmp < (zmid + particleSlab[jParticle] + halfHeight)) && (ztmp > (zmid + particleSlab[jParticle] - halfHeight))) {
          // std::cout << "one particle added" << std::endl;
          out[particleSlabIndex[jParticle]][iFrame] += 1;
        }
      }
    }
  }

  out.output(pattern);

  return 0;
}
