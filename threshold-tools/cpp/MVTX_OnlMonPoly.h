#include "TH2Poly.h"

#include <cmath>

const int NLAYERS = 3;
const int NStaves[3] = {12, 16, 20};
const float StartAngle[3] = {
    16.997 / 360 * (M_PI * 2.), 17.504 / 360 * (M_PI * 2.),
    17.337 / 360 * (M_PI * 2.)};  // start angle of first stave in each layer
const float MidPointRad[3] = {23.49, 31.586, 39.341};
const int mapstave[3][20] = {
    {6, 5, 4, 3, 2, 1, 12, 11, 10, 9, 8, 7, 0, 0, 0, 0, 0, 0, 0, 0},
    {20, 19, 18, 17, 16, 15, 14, 13, 28, 27,
     26, 25, 24, 23, 22, 21, 0, 0, 0, 0},
    {38, 37, 36, 35, 34, 33, 32, 31, 30, 29,
     48, 47, 46, 45, 44, 43, 42, 41, 40, 39}};

const int mapstave_flat[48] = {6, 5, 4, 3, 2, 1, 12, 11, 10, 9, 8, 7,
                               20, 19, 18, 17, 16, 15, 14, 13, 28, 27, 26, 25,
                               24, 23, 22, 21, 38, 37, 36, 35, 34, 33, 32, 31,
                               30, 29, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39};

inline void getStavePoint(int layer, int stave, double *px, double *py)
{
  float stepAngle = M_PI * 2 / NStaves[layer];               // the angle between to stave
  float midAngle = StartAngle[layer] + (stave * stepAngle);  // mid point angle
  float staveRotateAngle =
      M_PI / 2 -
      (stave *
       stepAngle);  // how many angle this stave rotate(compare with first stave)
  px[1] = MidPointRad[layer] *
          std::cos(midAngle);  // there are 4 point to decide this TH2Poly bin
                               // 0:left point in this stave;
                               // 1:mid point in this stave;
                               // 2:right point in this stave;
                               // 3:higher point int this stave;
  py[1] =
      MidPointRad[layer] * std::sin(midAngle);  // 4 point calculated accord the
                                                // blueprint roughly calculate
  px[0] = 7.7 * std::cos(staveRotateAngle) + px[1];
  py[0] = -7.7 * std::sin(staveRotateAngle) + py[1];
  px[2] = -7.7 * std::cos(staveRotateAngle) + px[1];
  py[2] = 7.7 * std::sin(staveRotateAngle) + py[1];
  px[3] = 5.623 * std::sin(staveRotateAngle) + px[1];
  py[3] = 5.623 * std::cos(staveRotateAngle) + py[1];
}

inline void createPoly(TH2Poly *h)
{
  for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
  {
    for (int istave = 0; istave < NStaves[ilayer]; istave++)
    {
      double *px = new double[4];
      double *py = new double[4];
      getStavePoint(ilayer, istave, px, py);
      for (int icoo = 0; icoo < 4; icoo++)
      {
        px[icoo] /= 10.;
        py[icoo] /= 10.;
      }
      h->AddBin(4, px, py);
      delete[] px;
      delete[] py;
    }
  }
}
