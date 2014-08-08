#include "seamcarve.hpp"

#include <algorithm>
#include <deque>
#include <iostream>
using std::max_element;
using std::min_element;
using std::min;
using std::pair;
using std::deque;
using std::vector;


namespace seamcarve {

   /**********************DECLARATIONS***********************/

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

   QImage remove_row(const QImage image);

   float* calculate_energy(const QImage image);

   float calculate_pixel_energy(const QImage image, int x, int y);

   QColor* calculate_energy_colors(float* energies,
                                   int num_pixels,
                                   QColor start_color,
                                   QColor end_color,
                                   float min_energy,
                                   float max_energy);

   template <typename T, typename S>
   void copy_and_prune_data(T* old_data, T* data, int old_data_size, S& indexes);

   void image_cleanup_handler(void *data);

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

      if (width_diff < 0) {
         result = remove_columns(result, -width_diff);
      }

      return result;
   }

   QImage calculate_energy_image(const QImage image) {
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
      QColor* colors = calculate_energy_colors(energies, num_pixels, start_color,
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
      float* energies = calculate_energy(image); // Per pixel energy
      float energy_diffs[num_pixels];            // tracks diffs in energy between pixel.
      int prev_pixels[num_pixels];               // NxM pixels that point to prev with min energy.
      deque<int> seam_pixels;                    // N pixels that are to remove

      QRgb* image_data = NULL;
      for (int i = 0; i < num; i++) {
         // calculate energies from previous energies.
         if (i > 0) {
            int num_pixels_pruned = num_pixels - height;
            float* energies_pruned = new float[num_pixels_pruned];
            copy_and_prune_data(energies, energies_pruned, num_pixels, seam_pixels);
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

         copy_and_prune_data(old_image_data, image_data, num_pixels, seam_pixels);

         if (!get_old_image_data_from_original) {
            delete old_image_data;
         }
      }

      // free memory
      delete energies;

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
   }

   /*
    * Walks the NxM energy_diff grid to find the already calculated seam.
    */
   void find_seam_by_col(int width, int height, deque<int>& seam_pixels, float* energy_diffs, int* prev_pixels) {
      // working from the last row down to the first find the seam of min values.
      float* min_diff = min_element(energy_diffs + (height - 1) * width,
                                    energy_diffs + height * width);
      // set seam pixels
      int min_pixel = min_diff - energy_diffs;
      for (int row = height - 1; row >= 0; row--) {
         seam_pixels.push_front(min_pixel);
         min_pixel = prev_pixels[min_pixel];
      }
   }

   /*
    * TODO
    */
   QImage remove_row(const QImage image) {
      return image;
   }

   /*
    * Calculate the energy of all pixels returning a 1-dimensional
    * array of size row*col.
    * The ordering is row major, which means (0r,0c), (0r,1c)...(Nr,Mc)
    *
    * Caller must free this energy array.
    */
   float* calculate_energy(const QImage image) {
      int width = image.width();
      int height = image.height();
      float* energies = new float[width * height];

      for (int row = 0; row < height; row++) {
         for (int col = 0; col < width; col++) {
            float energy = calculate_pixel_energy(image, col, row);
            energies[row * width + col] = energy;
         }
      }

      return energies;
   }

   /*
    * Calculate pixel energy based on difference in neighboring RGB values.
    */
   inline float calculate_pixel_energy(const QImage image, int x, int y) {
      // IMPORTANT: image must be const or this will make a deep copy.
      QRgb* pixels = (QRgb*) image.bits();

      int width    = image.width();
      int height   = image.height();
      QRgb pixel   = pixels[y * width + x];
      uint red     = qRed(pixel);
      uint green   = qGreen(pixel);
      uint blue    = qBlue(pixel);

      float energy = 0.0f;
      uint num_neighbors = 0;
      for (int i = x - 1; i <= x + 1; i++) {
         for (int j = y - 1; j <= y + 1; j++) {
            // neighboring pixel check
            if (i < 0 || i >= width) continue;
            if (j < 0 || j >= height) continue;
            if (i == x && j == y) continue;

            num_neighbors++;

            QRgb rgb = pixels[j * width + i];
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
   QColor* calculate_energy_colors(float* energies,
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

   /*
    * Copies old_data to data, but ignores values at index offets.
    * Assumes that the indexes are sorted.
    */
   template <typename T, typename S>
   void copy_and_prune_data(T* old_data, T* data, int old_data_size, S& indexes) {
      int prev_index = 0;
      for (int i = 0; i < indexes.size(); i++) {
         int index = indexes[i];
         int amount = index - prev_index;

         memcpy(data + prev_index - i, old_data + prev_index, amount * sizeof(T)); 

         prev_index = index;
      }

      // copy last chunk
      memcpy(data + prev_index - indexes.size(), old_data + prev_index,
            (old_data_size - prev_index) * sizeof(T)); 
   }

   // deletes memory buffer from QImages
   void image_cleanup_handler(void *data) {
      delete ((QRgb*) data);
   }
}
