#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <QtGui/QImage>
#include <functional>
   using std::function;

namespace seamcarve {

   /**
    * Represents a pixel chain.
    *
    * A prev_pixel_index < 0 means, there is no previous.
    */
   struct Stitch {
      Stitch() : energy(0), prev_pixel_index(-1) {};
      Stitch(const Stitch& s) : energy(s.energy), prev_pixel_index(s.prev_pixel_index) {};
      Stitch(float e) : energy(e), prev_pixel_index(-1) {};
      Stitch(float e, int ppi) : energy(e), prev_pixel_index(ppi) {};

      float energy;
      int prev_pixel_index;
   };

   /*
    * Data per pixel that is passed during image
    * mapping transformations.
    * See map, imap, transform_image.
    */
   struct PixelArgs {
      PixelArgs(const QImage image) {
         width      = image.width();
         height     = image.height();
         num_pixels = width * height;
         pixels     = (QRgb*) image.bits();
      }

      void set_pixel(int _x, int _y) {
         x = _x;
         y = _y;
         pixel_index = y * width + x;
      }

      int x;
      int y;
      int pixel_index;
      int width;
      int height;
      int num_pixels;
      QRgb* pixels;
   };

   // Useful types for more easily declaring lambdas as functions. Good for map, imap.
   typedef function<QRgb(PixelArgs&)> RGBfn;
   typedef function<Stitch(PixelArgs&,Stitch*)> StitchPartial;

   /* Helper templates that allow iterating over the pixel data and returning new data.
    * Think of this like map in scala or python, just restricted to iterating over images.
    *   create_img: There are methods for taking an image and returning a new image.
    *   map: There are methods for taking an image and returning some data type T*
    *   imap: And finally there are some, in-place, versions that allow the return T* to be passed in,
    *         for efficiency reasons.
    *   pmap: There are methods that are like above, but allow access to the final return values T*.
    *         This is useful when the mapping function needs progressive look-behind access to the final
    *         result.
    *   pimap: pmap and imap combined.  Progressive and in-place.
    *    
    * NOTE: This is hard to grok, but simplies the code.  There are usually two versions of each function
    *       to allow passing a function ptr or the function type. Why doesn't cpp allow lambdas being 
    *       passed as an argument?
    */
   template <typename T> T* map(const QImage image, function<T(PixelArgs&)> transform);
   template <typename T> T* map(const QImage image, T (*transform)(PixelArgs&) );
   template <typename T> T* imap(const QImage image, T* output, function<T(PixelArgs&)> transform);
   template <typename T> T* imap(const QImage image, T* output, T (*transform)(PixelArgs&) );
   template <typename T> T* pmap(const QImage image, function<T(PixelArgs&,T*)> transform);
   template <typename T> T* pmap(const QImage image, T (*transform)(PixelArgs&,T*) );
   template <typename T> T* pimap(const QImage image, T* output, function<T(PixelArgs&,T*)> transform);
   template <typename T> T* pimap(const QImage image, T* output, T (*transform)(PixelArgs&,T*) );
   QImage create_img(const QImage image, RGBfn transform);
   QImage create_img(const QImage image, QRgb (*transform)(PixelArgs&) );

   // copying old_data to data, while ignoring indexes.  Like set difference.
   template <typename T, typename S> T* prune(T* old_data, S& indexes, int old_data_size);
   template <typename T, typename S> T* prune(T* old_data, T* data, S& indexes, int old_data_size);

   // Qt requires this to cleanup images.
   void image_cleanup_handler(void *data);

}

// template definitions
#include "utility.tpp"

#endif
