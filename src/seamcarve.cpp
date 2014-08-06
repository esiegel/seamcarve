#include "seamcarve.hpp"

#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;
using std::max;
using std::min;



namespace seamcarve {

   /**********************PRIVATE***********************/

   float calculate_pixel_energy(int x, int y, QImage image);
   QColor calculate_pixel_color(float energy,
                                QColor start_color,
                                QColor end_color,
                                float min_energy,
                                float max_energy);


   /**********************PUBLIC***********************/

   QImage do_seam_carving(QImage image, QSize old_size, QSize size) {
      cout << "DO SEEM CARVING" << endl;
      return QImage();
   }

   QImage calculate_image_energy(QImage image) {
      int width = image.width();
      int height = image.height();
      float max_energy = 0;
      float min_energy = std::numeric_limits<float>::max();
      float energies[width * height];

      QImage energy_image(width, height, image.format());

      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {
            float energy = calculate_pixel_energy(col, row, image);
            energies[row * width + col] = energy;
            min_energy = std::min(energy, min_energy);
            max_energy = std::max(energy, max_energy);
         }
      }

      QColor start_color = QColor("blue");
      QColor end_color = QColor("orange");

      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {
            QColor color = calculate_pixel_color(energies[row * width + col],
                                                 start_color,
                                                 end_color,
                                                 min_energy,
                                                 max_energy);

            energy_image.setPixel(col, row, color.rgb());
         }
      }

      return energy_image;
   }

   /**********************PRIVATE***********************/

   float calculate_pixel_energy(int x, int y, QImage image) {
      QRgb pixel = image.pixel(x, y);
      uint red   = qRed(pixel);
      uint green = qGreen(pixel);
      uint blue  = qBlue(pixel);

      float energy = 0.0f;
      uint num_neighbors = 0;
      for (int i = x - 1; i <= x + 1; i++) {
         for (int j = y - 1; j <= y + 1; j++) {
            // neighboring pixel check
            if (i < 0 || i >= image.width()) continue;
            if (j < 0 || j >= image.height()) continue;
            if (i == x && j == y) continue;

            num_neighbors++;

            QRgb rgb = image.pixel(i, j);
            energy += abs(red - qRed(rgb))
                      + abs(green - qGreen(rgb))
                      + abs(blue - qBlue(rgb));
         }
      }

      return energy / num_neighbors;
   }

   /*
    * Chooses pixel color based on linear interpolation
    * of energy.
    */
   QColor calculate_pixel_color(float energy,
                               QColor start_color,
                               QColor end_color,
                               float min_energy,
                               float max_energy) {
      int sred, sblue, sgreen;
      int ered, eblue, egreen;

      start_color.getRgb(&sred, &sgreen, &sblue);
      end_color.getRgb(&ered, &egreen, &eblue);

      float percent = (energy - min_energy) / (max_energy - min_energy);

      int red   = sred   + ((ered - sred) * percent);
      int green = sgreen + ((egreen - sgreen) * percent);
      int blue  = sblue  + ((eblue - sblue) * percent);

      return QColor(red, green, blue);
   }


}
