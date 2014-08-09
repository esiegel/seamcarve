#include "seamcarve.hpp"
#include "utility.hpp"

#include <QtGui/QTransform>
#include <algorithm>
   using std::max_element;
   using std::min_element;
#include <deque>
   using std::deque;
#include <iostream>
   using std::cout;
   using std::endl;
#include <vector>
   using std::vector;


namespace seamcarve {

   /**********************DECLARATIONS***********************/

   QImage remove_rows(const QImage image, int num);

   QImage remove_columns(const QImage image, int num);

   void calculate_energy_diff_by_col(int width,
                                         int height,
                                         float* energies,
                                         float* energy_diffs,
                                         int* prev_pixels);

   void find_seam_by_col(int width,
                         int height,
                         deque<int>& seam_pixels,
                         float* energy_diffs,
                         int* prev_pixels);

   float calculate_pixel_energy(PixelArgs& pargs);

   QColor calculate_energy_color(float energy,
                                 float min_energy,
                                 float max_energy,
                                 QColor start_color,
                                 QColor end_color);

   /**********************PUBLIC***********************/

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
      int num_pixels   = image.width() * image.height();
      float* energies  = map(image, calculate_pixel_energy);
      float min_energy = *min_element(energies, energies + num_pixels);
      float max_energy = *max_element(energies, energies + num_pixels);

      // Calculate Pixel colors
      RGBfn transform = [energies, min_energy, max_energy](PixelArgs& pargs) {
         float energy = energies[pargs.pixel_index];
         return calculate_energy_color(energy, min_energy, max_energy,
                                       QColor("blue"), QColor("orange")).rgb();
      };

      QImage energy_image = img_map(image, transform);

      // free memory
      delete energies;

      return energy_image;
   }

   /**********************PRIVATE***********************/

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
      int width = image.width();
      int height = image.height();
      int num_pixels = width * height;

      //Energy information.  Allocated on the heap due to large size.
      float* energies = map(image, calculate_pixel_energy); // Per pixel energy
      float* energy_diffs = new float[num_pixels]; // tracks diffs in energy between pixel.
      int* prev_pixels = new int[num_pixels];      // NxM pixels that point to prev with min energy.
      deque<int> seam_pixels;                      // N pixels that are to remove

      QRgb* image_data = NULL;
      for (int i = 0; i < num; i++) {

         // calculate energies from previous energies.
         if (i > 0) {
            int num_pixels_pruned = num_pixels - height;
            float* energies_pruned = new float[num_pixels_pruned];
            copy_and_prune(energies, energies_pruned, num_pixels, seam_pixels);
            num_pixels = num_pixels_pruned;

            seam_pixels.clear();
            delete energies;

            energies = energies_pruned;
         }

         // calculates energy_diffs and tracks prev min pixels as well.
         // would prefer to separate the prev_pixel calculation, but it
         // saves much work to calculate together.
         calculate_energy_diff_by_col(width - i, height, energies, energy_diffs, prev_pixels);

         // traverse the grid of prev_pixels and find the seam.
         find_seam_by_col(width - i, height, seam_pixels, energy_diffs, prev_pixels);

         // remove old pixel values specified in the seam.
         bool get_old_image_data_from_original = image_data == NULL;
         QRgb* old_image_data = (get_old_image_data_from_original) 
                                ? (QRgb*) image.bits()
                                : image_data;

         image_data = new QRgb[num_pixels - height];

         copy_and_prune(old_image_data, image_data, num_pixels, seam_pixels);

         if (!get_old_image_data_from_original) {
            delete old_image_data;
         }
      }

      // free memory
      delete energies;
      delete energy_diffs;
      delete prev_pixels;

      return QImage((uchar*) image_data,
                    width - num,
                    height,
                    image.format(),
                    image_cleanup_handler,
                    image_data);
   }

   void calculate_energy_diff_by_col(int width,
                                     int height,
                                     float* energies,
                                     float* energy_diffs,
                                     int* prev_pixels) {

      // set each energy diff to the min of neighbor diffs.
      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {

            // set first row of energy diffs to be just the energy of that row.
            if (row == 0) {
               energy_diffs[col] = energies[col];
               continue;
            }

            // calculate the minimum energy from the previous row and record trail.
            float min_energy = std::numeric_limits<float>::max();
            for (int prev_col = col - 1; prev_col <= col + 1; prev_col++) {
               if (prev_col < 0 || prev_col >= width) continue;

               int prev_pixel = (row - 1) * width + prev_col;
               float prev_energy = energy_diffs[prev_pixel];
               if (prev_energy <= min_energy) {
                  prev_pixels[row * width + col] = prev_pixel;
                  min_energy = prev_energy;
               }
            }

            int pixel = row * width + col;
            energy_diffs[pixel] = energies[pixel] + min_energy;
         }
      }
   }

   /*
    * Walks the NxM energy_diff grid to find the already calculated seam.
    */
   void find_seam_by_col(int width, int height, deque<int>& seam_pixels,
                         float* energy_diffs, int* prev_pixels) {

      // find min energy in last row.
      int last_row_offset   = (height - 1) * width;
      float* min_energy_ptr = min_element(energy_diffs + last_row_offset,
                                          energy_diffs + height * width);

      // set seam pixels
      int pixel_index = min_energy_ptr - energy_diffs;
      for (int row = height - 1; row >= 0; row--) {
         seam_pixels.push_front(pixel_index);
         pixel_index = prev_pixels[pixel_index];
      }
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
            energy += abs(red - qRed(rgb))
                      + abs(green - qGreen(rgb))
                      + abs(blue - qBlue(rgb));
         }
      }

      return energy / num_neighbors;
   }

   /*
    * Chooses pixel color based on linear interpolation
    * of start and end colors.
    */
   QColor calculate_energy_color(float energy,
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
