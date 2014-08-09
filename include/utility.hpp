#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <QtGui/QImage>
#include <functional>
   using std::function;

namespace seamcarve {

   /*
    * Data per pixel that is passed during image
    * transformatino.
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

   // Useful function typedefs for image transformations.
   typedef function<QRgb(PixelArgs&)> RGBfn;
   typedef function<float(PixelArgs&)> Floatfn;

   // map lambdas and function to image transformations.
   template <typename T> T* map(const QImage image, function<T(PixelArgs&)> transform);
   template <typename T> T* map(const QImage image, T (*transform)(PixelArgs&) );
   QImage img_map(const QImage image, RGBfn transform);
   QImage img_map(const QImage image, QRgb (*transform)(PixelArgs&) );

   // copying old_data to data, while ignoring indexes.
   template <typename T, typename S>
   void copy_and_prune(T* old_data, T* data, int old_data_size, S& indexes);

   // Qt requires this to cleanup images.
   void image_cleanup_handler(void *data);

}

#include "utility.tpp"

#endif
