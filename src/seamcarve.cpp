#include "seamcarve.hpp"

#include <algorithm>
#include <iostream>
using std::max_element;
using std::min_element;
using std::min;
using std::pair;
using std::vector;



namespace seamcarve {

   /**********************PRIVATE***********************/

   float* calculate_energy(QImage image);

   float calculate_pixel_energy(int x, int y, QImage image);

   QColor* calculate_pixel_colors(float* energies,
                                  int num_pixels,
                                  QColor start_color,
                                  QColor end_color,
                                  float min_energy,
                                  float max_energy);

   QImage remove_column(QImage image);

   QImage remove_row(QImage image);

   QImage remove_seam(QImage image, int seam[], bool column);

   /**********************PUBLIC***********************/

   /*
    * Using the seamcarve algorithm resize the image.
    * Only supports shrinking, not expanding.
    * Removes rows then columns, though in the actual paper
    * this ordering is mathematically calculated.
    */
   QImage resize(QImage image, QSize size) {
      QImage result   = image;
      int width_diff  = size.width() - image.width();
      int height_diff = size.height() - image.height();

      for (int i = height_diff; i < 0; i++) {
         result = remove_row(result);
      }

      for (int i = width_diff; i < 0; i++) {
         result = remove_column(result);
      }

      return result;
   }

   QImage calculate_energy_image(QImage image) {
      int width           = image.width();
      int height          = image.height();
      int num_pixels      = width * height;
      QImage energy_image = QImage(width, height, image.format());
      QColor start_color  = QColor("blue");
      QColor end_color    = QColor("orange");

      // Calculate energies, min, and max
      float* energies  = calculate_energy(image);
      float max_energy = *max_element(energies, energies + num_pixels);
      float min_energy = *min_element(energies, energies + num_pixels);

      // Set Colors
      QColor* colors = calculate_pixel_colors(energies, num_pixels, start_color,
                                              end_color, min_energy, max_energy);

      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {
            QRgb rgb = colors[row * width + col].rgb();
            energy_image.setPixel(col, row, rgb);
         }
      }

      // free memory
      delete colors;
      delete energies;

      return energy_image;
   }

   /**********************PRIVATE***********************/

   QImage remove_column(QImage image) {
      int width       = image.width();
      int height      = image.height();
      int num_pixels  = width * height;
      float* energies = calculate_energy(image);

      int prev_pixels[width * height]; // NxM pairs that point to prev row with min energy
      int seam_pixels[height];         // N pairs that are the pixels to remove

      // used to track differences in energies between row to row.
      float energy_diffs[num_pixels];

      // set first row of energy diffs to be just the energy of that row.
      for (int i = 0; i < width; i++) {
         energy_diffs[i] = energies[i];
      }

      // set each energy diff to the min of neighbor diffs.
      for (int row = 1; row < height; row++) {
         for (int col = 0; col < width; col++) {

            // keep track of minimum energy from previous row
            float min_energy    = std::numeric_limits<float>::max();
            int pixel           = row * width + col;
            int prev_row_offset = pixel - height - col;

            // calculate the minimum energy from the previous row and record trail.
            for (int prev_col = col - 1; prev_col <= col + 1; prev_col++) {
               if (prev_col < 0 || prev_col >= width) continue;

               float prev_energy = energy_diffs[prev_row_offset + prev_col];
               if (prev_energy < min_energy) {
                  prev_pixels[row * width + col] = (row - 1) * width + prev_col;
                  min_energy = prev_energy;
               }
            }

            energy_diffs[pixel] = energies[pixel] + min_energy;
         }
      }

      // working from the last row down to the first find the seam of min values.
      float* min_diff = min_element(energy_diffs + (height - 1) * width,
                                    energy_diffs + height * width);
      int min_pixel = min_diff - energy_diffs;

      // set seam pixels
      for (int row = height - 1; row >= 0; row--) {
         seam_pixels[row] = min_pixel;
         min_pixel = prev_pixels[min_pixel];
      }

      // free memory
      delete energies;

      return remove_seam(image, seam_pixels, true);
   }

   /*
    * TODO
    */
   QImage remove_row(QImage image) {
      return image;
   }

   /*
    * Copies and manipulates the underlying image data to create
    * a new image.  Assumes that the size of seam is the height/width
    * of the image and that it is sorted from lowest to highest pixel.
    */
   QImage remove_seam(QImage image, int seam[], bool column) {
      // calculate new size of image
      QSize old_size = image.size();
      int seam_size  = column ? old_size.height() : old_size.width();
      int width      = old_size.width() - (column ? 1 : 0);
      int height     = old_size.height() - (column ? 0 : 1);
      int num_pixels = width * height;

      // src and dst image data.
      QRgb* old_data = (QRgb*) image.bits();
      QRgb* data     = new QRgb[num_pixels]; // TODO how does this memory get freed?

      int prev_pixel = 0;
      for (int i = 0; i < seam_size; i++) {
         int seam_pixel = seam[i];
         int amount = seam_pixel - prev_pixel;

         memcpy(data + prev_pixel - i, old_data + prev_pixel, amount* sizeof(QRgb)); 

         prev_pixel = seam_pixel;
      }

      // new image
      return QImage((uchar*)data, width, height, image.format());
   }

   /*
    * Calculate the energy of all pixels returning a 1-dimensional
    * array of size row*col.
    * The ordering is row major, which means (0r,0c), (0r,1c)...(Nr,Mc)
    *
    * Caller must free this energy array.
    */
   float* calculate_energy(QImage image) {
      int width = image.width();
      int height = image.height();
      float* energies = new float[width * height];

      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {
            float energy = calculate_pixel_energy(col, row, image);
            energies[row * width + col] = energy;
         }
      }

      return energies;
   }

   /*
    * Calculate pixel energy based on difference in neighboring RGB values.
    */
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
   QColor* calculate_pixel_colors(float* energies,
                                  int num_pixels,
                                  QColor start_color,
                                  QColor end_color,
                                  float min_energy,
                                  float max_energy) {

      QColor* colors = new QColor[num_pixels];
      int sred, sblue, sgreen;
      int ered, eblue, egreen;
      float energy_range = max_energy - min_energy;

      start_color.getRgb(&sred, &sgreen, &sblue);
      end_color.getRgb(&ered, &egreen, &eblue);

      for (int i = 0; i < num_pixels; i++) {
         float energy = energies[i];
         float percent = (energy - min_energy) / energy_range;

         int red   = sred   + ((ered - sred) * percent);
         int green = sgreen + ((egreen - sgreen) * percent);
         int blue  = sblue  + ((eblue - sblue) * percent);
         colors[i] = QColor(red, green, blue);
      }

      return colors;
   }
}
