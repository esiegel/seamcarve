#include "utility.hpp"

namespace seamcarve {

   /*
    * Create a new image by mapping transform function over each pixel.
    * Iterates in row major form.
    */
   QImage img_map(const QImage image, RGBfn transform) {
      QRgb* data = map(image, transform);
      return QImage((uchar*) data, image.width(), image.height(),
                    image.format(), image_cleanup_handler, data);
   }

   QImage img_map(const QImage image, QRgb (*transform)(PixelArgs&) ) {
      function<QRgb(PixelArgs&)> fn = transform;
      return img_map(image, fn);
   }

   // used a callback in QIMage to delete the memory buffer. 
   void image_cleanup_handler(void *data) {
      delete ((QRgb*) data);
   }

}
