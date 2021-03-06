#include "seamcarve.hpp"
#include "utility.hpp"

#include <QtGui/QTransform>
#include <algorithm>
   using std::minmax_element;
   using std::min_element;
   using std::min;
   using std::max;
#include <cmath>
   using std::fabs;
#include <deque>
   using std::deque;
#include <functional>
   using std::bind;
   using std::function;
   using std::placeholders::_1;
   using std::placeholders::_2;
#include <utility>
   using std::pair;
#include <vector>
   using std::vector;

#include <iostream>

namespace seamcarve {

   /**********************INTERNAL DECLARATIONS***********************/

   QImage remove_rows(const QImage image, int num);

   QImage remove_columns(const QImage image, int num);

   float* calculate_min_energies(const QImage image, float* energies);

   deque<int> find_column_seam(float* energies, int width, int height);

   float calculate_pixel_energy(PixelArgs& pargs);

   QColor calculate_color(float energy,
                          float min_energy,
                          float max_energy,
                          QColor start_color,
                          QColor end_color);

   /**********************DEFINITIONS***********************/

   /*
    * Using the seamcarve algorithm resize the image.
    * Only supports shrinking, not expanding.
    * Removes rows then columns, though in the actual paper
    * this ordering is mathematically calculated.
    */
   QImage resize(const QImage image, QSize size) {
      QImage result = image;
      int width_diff = size.width() - image.width();
      int height_diff = size.height() - image.height();

      if (width_diff < 0) {
         result = remove_columns(result, -width_diff);
      }

      if (height_diff < 0) {
         result = remove_rows(result, -height_diff);
      }

      return result;
   }

   QImage calculate_energy_image(const QImage image) {
      // Calculate energies, min, and max
      int num_pixels     = image.width() * image.height();
      float* energies    = map(image, calculate_pixel_energy);
      auto minmax_energy = minmax_element(energies, energies + num_pixels);
      float min_energy   = *minmax_energy.first;
      float max_energy   = *minmax_energy.second;

      // Calculate Pixel colors
      RGBfn transform = [energies, min_energy, max_energy](PixelArgs& pargs) {
         float energy = energies[pargs.pixel_index];
         return calculate_color(energy, min_energy, max_energy,
                                QColor("blue"), QColor("orange")).rgb();
      };

      QImage energy_image = create_img(image, transform);

      // free memory
      delete energies;

      return energy_image;
   }

   /**********************INTERNAL DEFINITIONS***********************/

   /*
    * Remove row by removing columns of the transposed image.
    * This approach is slower, but cleaner.
    */
   QImage remove_rows(const QImage image, int num) {
      QTransform rotate_forward  = QTransform().rotate(90);
      QTransform rotate_backward = QTransform().rotate(-90);

      QImage rotated_image = remove_columns(image.transformed(rotate_forward), num);
      return rotated_image.transformed(rotate_backward);
   }


   /*
    * Calculate a new image by removing least energetic pixel seams.
    *
    * Flow
    *   Img -> Eng -> Eng_diff -> Seam
    *   Img + Seam -> Img_small
    *
    * Calculating the energy of all pixels is the bottleneck so we reuse 
    * previously calculated energies as follows:
    *
    * Flow
    *   Eng + Seam -> Eng_small -> Eng_diff_small -> Seam_small
    */
   QImage remove_columns(const QImage image, int num) {
      QRgb* image_data = (QRgb*) image.bits();
      deque<int> seam;

      for (int i = 0; i < num; i++) {
         int width      = image.width() - i;
         int height     = image.height();
         int num_pixels = width * height;

         // create an image used for iteration.  We can't simply use the image above as it is also const.
         // removing the constness, we would incur a copy, when we access the underlying bits.
         const QImage prev_image = QImage((uchar*) image_data, width, height, image.format());
         float* energies         = map(image, calculate_pixel_energy); // Per pixel energy
         float* min_energies     = calculate_min_energies(prev_image, energies);

         // traverse the grid of prev_pixels and find the seam.
         seam = find_column_seam(min_energies, width, height);

         // actually remove seam pixels
         QRgb* prev_image_data = (QRgb*) prev_image.bits();
         image_data            = prune(prev_image_data, seam, num_pixels);

         // only delete image data that was copied from the origial image.
         if (i > 0) delete prev_image_data;

         // free memory
         delete[] min_energies;
         delete energies;
      }

      return QImage((uchar*) image_data, image.width() - num, image.height(),
                    image.format(), image_cleanup_handler, image_data);
   }

   /*
    * Determine min energy of the current pixel based on looking at previous neighbor pixels
    */
   float* calculate_min_energies(const QImage image, float* energies) {
      int width      = image.width();
      int height     = image.height();
      int num_pixels = width * height;

      // results
      float* min_energies = new float[num_pixels];

      for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
          int pixel_index = (row * width) + col;

          // first row of diff should just be energy of pixel.
          if (row == 0) {
            min_energies[pixel_index] = energies[pixel_index];
            continue;
          }

          float min_prev_energy = std::numeric_limits<float>::max();
          for (int prev_col = col - 1; prev_col <= col + 1; prev_col++) {
             if (prev_col < 0 || prev_col >= width) continue;

             int prev_pixel_index = (row - 1) * width + prev_col;
             int prev_energy      = energies[prev_pixel_index];

             if (prev_energy <= min_prev_energy) {
                min_prev_energy = prev_energy;
             }
          }

          min_energies[pixel_index] = energies[pixel_index] + min_prev_energy;
        }
      }

      return min_energies;
   }

   /*
    * Walks the NxM energy_diff grid to find the already calculated seam.
    */
   deque<int> find_column_seam(float* min_energies, int width, int height) {
      deque<int> seam;

      int min_col = 0;

      for (int row = (height - 1); row >= 0; row--)  {
        int col;
        int col_high;

        // for the last row we need to search all columns, for others just neighbors
        if (row == height - 1) {
          col      = 0;
          col_high = width - 1;
        } else {
          col      = max(0, min_col - 1);
          col_high = min(width - 1, min_col + 1);
        }

        int min_col_energy = std::numeric_limits<float>::max();

        for (; col <= col_high; col++) {
          int index = (width * row) + col;
          float energy = min_energies[index];

          if (energy < min_col_energy) {
            min_col_energy = energy;
            min_col = col;
          }
        }

        seam.push_front((width * row) + min_col);
      }

      return seam;
   }

   /*
    * Calculate pixel energy based on difference
    * in neighboring RGB values.
    */
   float calculate_pixel_energy(PixelArgs& pargs) {
      QRgb pixel = pargs.pixels[pargs.pixel_index];
      uint red   = qRed(pixel);
      uint green = qGreen(pixel);
      uint blue  = qBlue(pixel);

      float energy = 0.0f;
      uint num_neighbors = 0;
      for (int i = pargs.x - 1; i <= pargs.x + 1; i++) {
         for (int j = pargs.y - 1; j <= pargs.y + 1; j++) {
            // neighboring pixel check
            if (i < 0 || i >= pargs.width) continue;
            if (j < 0 || j >= pargs.height) continue;
            if (i == pargs.x && j ==pargs.y) continue;

            num_neighbors++;

            QRgb rgb = pargs.pixels[j * pargs.width + i];
            energy += fabs(red - qRed(rgb))
                      + fabs(green - qGreen(rgb))
                      + fabs(blue - qBlue(rgb));
         }
      }

      return energy / num_neighbors;
   }

   /*
    * Chooses pixel color based on linear interpolation
    * of start and end colors.
    */
   QColor calculate_color(float energy,
                          float min_energy,
                          float max_energy,
                          QColor start_color,
                          QColor end_color) {
      int sred, sblue, sgreen;
      int ered, eblue, egreen;
      start_color.getRgb(&sred, &sgreen, &sblue);
      end_color.getRgb(&ered, &egreen, &eblue);

      float energy_range = max_energy - min_energy;
      float percent = (energy - min_energy) / energy_range;

      int red   = sred   + ((ered - sred) * percent);
      int green = sgreen + ((egreen - sgreen) * percent);
      int blue  = sblue  + ((eblue - sblue) * percent);
      return QColor(red, green, blue);
   }
}
