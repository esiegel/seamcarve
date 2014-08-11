#include "seamcarve.hpp"
#include "utility.hpp"

#include <QtGui/QTransform>
#include <algorithm>
   using std::max_element;
   using std::min_element;
#include <deque>
   using std::deque;
#include <functional>
   using std::bind;
   using std::function;
   using std::placeholders::_1;
   using std::placeholders::_2;
#include <iostream>
   using std::cout;
   using std::endl;
#include <utility>
   using std::pair;
#include <vector>
   using std::vector;


namespace seamcarve {

   /**********************INTERNAL DECLARATIONS***********************/

   QImage remove_rows(const QImage image, int num);

   QImage remove_columns(const QImage image, int num);

   Stitch calculate_stitch(PixelArgs& pargs, Stitch* stitches, float* energies);

   deque<int> find_column_seam(Stitch* stitches, int width, int height);

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
      int num_pixels   = image.width() * image.height();
      float* energies  = map(image, calculate_pixel_energy);
      float min_energy = *min_element(energies, energies + num_pixels);
      float max_energy = *max_element(energies, energies + num_pixels);

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
      int width = image.width();
      int height = image.height();
      int num_pixels = width * height;

      //Energy information.  Allocated on the heap due to large size.
      float* energies  = map(image, calculate_pixel_energy); // Per pixel energy
      Stitch* stitches = new Stitch[num_pixels];             // tracks energy differences and is used to find seams.
      QRgb* image_data = (QRgb*) image.bits();
      deque<int> seam;

      for (int i = 0; i < num; i++) {
         // create an image used for iteration.  We can't simply use the image above as it is also const.
         // removing the constness, we would incur a copy.
         const QImage prev_image = QImage((uchar*) image_data, width - i, height, image.format());

         // calculate energies from previous energies instead of recalculating.
         // Profiling showed that calculating energy was a bottleneck.
         if (i > 0) {
            float* energies_pruned = prune(energies, seam, num_pixels + seam.size());
            delete energies;
            energies = energies_pruned;
         }

         // calculates stitches using an progrressive, in-place map.
         function<Stitch(PixelArgs&,Stitch*)> stitch_partial = bind(calculate_stitch, _1, _2, energies);
         pimap(prev_image, stitches, stitch_partial);

         // traverse the grid of prev_pixels and find the seam.
         seam = find_column_seam(stitches, width - i, height);

         // actually remove seam pixels
         QRgb* prev_image_data = (QRgb*) prev_image.bits();
         image_data = prune(prev_image_data, seam, num_pixels);
         num_pixels = num_pixels - seam.size();

         // only delete image data that was copied from the origial image.
         if (i > 0) delete prev_image_data;
      }

      // free memory
      delete energies;
      delete stitches;

      return QImage((uchar*) image_data,
                    width - num,
                    height,
                    image.format(),
                    image_cleanup_handler,
                    image_data);
   }

   /*
    * Determine energy of the current pixel based on stitch energy
    * from the row above.
    */
   Stitch calculate_stitch(PixelArgs& pargs, Stitch* stitches, float* energies) {

      float energy = energies[pargs.pixel_index];

      // first row of diff should just be energy of pixel.
      if (pargs.y == 0) { return Stitch(energy, -1); }

      // calculate the minimum energy from the previous row and record trail.
      Stitch stitch = Stitch(std::numeric_limits<float>::max());
      for (int prev_col = pargs.x - 1; prev_col <= pargs.x + 1; prev_col++) {
         if (prev_col < 0 || prev_col >= pargs.width) continue;

         int prev_pixel_index = (pargs.y - 1) * pargs.width + prev_col;

         Stitch& prev_stitch = stitches[prev_pixel_index];
         if (prev_stitch.energy <= stitch.energy) {
            stitch.prev_pixel_index = prev_pixel_index;
            stitch.energy = prev_stitch.energy;
         }
      }

      stitch.energy += energy;
      return stitch;
   }

   /*
    * Walks the NxM energy_diff grid to find the already calculated seam.
    */
   deque<int> find_column_seam(Stitch* stitches, int width, int height) {
      // find min energy in last row.
      int last_row_offset    = (height - 1) * width;
      Stitch* min_stitch_ptr = min_element(stitches + last_row_offset,
                                           stitches + height * width,
                                           [](Stitch s1, Stitch s2) {return s1.energy < s2.energy;});
      deque<int> seam;

      // set seam pixels
      int pixel_index = min_stitch_ptr - stitches;
      for (int row = height - 1; row >= 0; row--) {
         seam.push_front(pixel_index);
         pixel_index = stitches[pixel_index].prev_pixel_index;
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
