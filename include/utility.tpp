#include <functional>
#include "utility.hpp"

namespace seamcarve {

   /*
    * Maps the transform function on each pixel of the image.
    */
   template <typename T>
   T* map(const QImage image, function<T(PixelArgs&)> transform) {
      T* output = new T[PixelArgs(image).num_pixels];
      return imap(image, output, transform);
   }

   template <typename T>
   T* map(const QImage image, T (*transform)(PixelArgs&) ) {
      function<T(PixelArgs&)> fn = transform;
      return map(image, fn);
   }

   /*
    * Maps the transform function on each pixel of the image.
    * Doesn't allocate new memory, does this in-place.
    */
   template <typename T>
   T* imap(const QImage image, T* output, function<T(PixelArgs&)> transform) {
      PixelArgs pargs = PixelArgs(image);

      for (int row = 0; row < pargs.height; row++) {
         for (int col = 0; col < pargs.width; col++) {
            pargs.set_pixel(col, row);
            output[pargs.pixel_index] = transform(pargs);
         }
      }

      return output;
   }

   template <typename T>
   T* imap(const QImage image, T* output, T (*transform)(PixelArgs&) ) {
      function<T(PixelArgs&)> fn = transform;
      return imap(image, output, fn);
   }

   /*
    * Progressive map which applies the transform function on each pixel
    * of the image and gives access to the final T* result.
    * Useful when the iteration needs to look-back at previously calculated
    * values.
    */
   template <typename T>
   T* pmap(const QImage image, function<T(PixelArgs&,T*)> transform) {
      T* output = new T[PixelArgs(image).num_pixels];
      return pimap(image, output, transform);
   }

   template <typename T>
   T* pmap(const QImage image, T (*transform)(PixelArgs&,T*) ) {
      function<T(PixelArgs&,T*)> fn = transform;
      return pmap(image, fn);
   }

   /*
    * Progressive and inplace map.
    * Used for efficiency and for look-back access to computed results.
    */
   template <typename T>
   T* pimap(const QImage image, T* output, function<T(PixelArgs&,T*)> transform) {
      using std::bind;
      using std::function;
      using namespace std::placeholders;

      // use std::bind to create a partially applied fn.
      function<T(PixelArgs&)> partial_fn = bind(transform, _1, output);

      return imap(image, output, partial_fn);
   }

   template <typename T>
   T* pimap(const QImage image, T* output, T (*transform)(PixelArgs&,T*) ) {
      function<T(PixelArgs&,T*)> fn = transform;
      return pimap(image, output, fn);
   }

   /*
    * Copies old_data to data, but ignores values specified in indexes.
    * Assumes that the indexes are sorted.
    */
   template <typename T, typename S>
   T* prune(T* old_data, T* data, S& indexes, int old_data_size) {
      // store original ptr to return.
      T* data_original = data;

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

      // return new data as a convenience.
      return data_original;
   }

   /*
    * Same as above, but creates new data.
    */
   template <typename T, typename S>
   T* prune(T* old_data, S& indexes, int old_data_size) {
      int data_size = old_data_size - indexes.size();
      return prune(old_data, new T[data_size], indexes, old_data_size);
   }

}
