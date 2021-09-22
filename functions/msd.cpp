#include "Base.hpp"
#include "displacement.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 3)) {
    std::cout << "input number should be two " << std::endl;
    std::cout << "msd folder out"
              << std::endl;
    return 0;
  }

  std::string foldername = argv[1];
  std::string outname = argv[2];

  Data data;
  data.readFolder(foldername);
  data.load();
  std::cout << "load successful" << std::endl;

  MSD_t msd(compute_correlated_displacement_property<MSD_t>(data));
  std::cout << "cal successful" << std::endl;
  Table res;
  // dt x y z x2 y2 z2 r2 count
  res.setcols(9);
  res.setrows(msd.dts.size());
  for (size_t i = 0; i < msd.dts.size(); i++) {
    res[0][i] = msd.dts[i];
    res[1][i] = msd.res[7 * i];
    res[2][i] = msd.res[7 * i + 1];
    res[3][i] = msd.res[7 * i + 2];
    res[4][i] = msd.res[7 * i + 3];
    res[5][i] = msd.res[7 * i + 4];
    res[6][i] = msd.res[7 * i + 5];
    res[7][i] = msd.res[7 * i + 6];
    res[8][i] = msd.counts[i];
  }

  std::cout << "output successful" << std::endl;
  res.output(outname);

  return 0;
}
