#include "Base.hpp"
// #include "displacement.hpp"
#include "fsqt.hpp"

// int main(int argc, char const* argv[]) {
//   if ((argc != 4)) {
//     std::cout << "input number should be two " << std::endl;
//     std::cout << "fsqt folder out q"
//               << std::endl;
//     return 0;
//   }

//   std::string foldername = argv[1];
//   std::string outname = argv[2];
//   double q = std::stod(argv[3]);

//   Data data;
//   data.readFolder(foldername);
//   data.load();
//   std::cout << "load successful" << std::endl;

//   Fsq_t f(fsqt(data, q));
//   std::cout << "cal successful" << std::endl;
//   Table res;
//   // dt x y z x2 y2 z2 r2 count
//   res.setcols(3);
//   res.setrows(f.dts.size());
//   for (size_t i = 0; i < f.dts.size(); i++) {
//     res[0][i] = f.dts[i];
//     res[1][i] = f.res[i];
//     res[2][i] = f.counts[i];
//   }

//   std::cout << "output successful" << std::endl;
//   res.output(outname);

//   return 0;
// }

int main(int argc, char const* argv[]) {
  if ((argc != 4)) {
    std::cout << "input number should be two " << std::endl;
    std::cout << "fsqt folder out q"
              << std::endl;
    return 0;
  }

  std::string foldername = argv[1];
  std::string outname = argv[2];
  double q = std::stod(argv[3]);

  Data data;
  data.readFolder(foldername);
  std::cout << "read successful" << std::endl;

  // cannot use right value?
  // size_t step = 20, low = 0, high = 20000;
  // data.load(step, low, high);
  data.load();
  std::cout << "load successful" << std::endl;

  Fsqt f(data, q);
  std::cout << "cal successful" << std::endl;
  Table res;
  std::cout << "size info:\n"
            << f.dts.size() << " "
            << f.res.size() << " "
            << f.counts.size() << " "
            << std::endl;
  // dt x y z x2 y2 z2 r2 count
  res.setcols(3);
  res.setrows(f.dts.size());

  for (size_t i = 0; i < f.dts.size(); i++) {
    res[0][i] = static_cast<double>(f.true_dts[i]);
    res[1][i] = f.res[i];
    res[2][i] = static_cast<double>(f.counts[i]);
  }

  // for (size_t i = 0; i < f.dts.size(); i++) {
  //   std::cout << f.true_dts[i] << " " << f.res[i] << " " << f.counts[i] << "\n";
  // }
  // std::cout << std::endl;
  std::cout << "output successful" << std::endl;
  res.output(outname);

  return 0;
}
