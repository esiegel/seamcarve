#include "utility.hpp"

namespace seamcarve {

   /*
    * Map transform function onto each pixel and returns result.
    * Iterates in row major form.
    */
   template <typename T>
   T* map(const QImage image, function<T(PixelArgs&)> transform) {
      PixelArgs pargs = PixelArgs(image);
      T* transformed_data = new T[pargs.num_pixels];

      for (int row = 0; row < pargs.height; row++) {
         for (int col = 0; col < pargs.width; col++) {
            pargs.set_pixel(col, row);
            transformed_data[pargs.pixel_index] = transform(pargs);
         }
      }

      return transformed_data;
   }

   template <typename T>
   T* map(const QImage image, T (*transform)(PixelArgs&) ) {
      function<T(PixelArgs&)> fn = transform;
      return map(image, fn);
   }


   /*
    * Copies old_data to data, but ignores values specified in indexes.
    * Assumes that the indexes are sorted.
    */
   template <typename T, typename S>
   void copy_and_prune(T* old_data, T* data, int old_data_size, S& indexes) {
      int start = 0;
      for (int index : indexes) {
         // amount to copy
         int amount = index - start;

         // copy data
         memcpy(data, old_data,  amount * sizeof(T)); 

         // increment pointers to next start.
         data += amount;
         old_data += amount + 1;

         start = index + 1;
      }

      // copy to the end
      memcpy(data, old_data, (old_data_size - start) * sizeof(T)); 
   }

}
