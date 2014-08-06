#ifndef SEAMCARVE_HPP
#define SEAMCARVE_HPP

#include <QtCore/QSize>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

namespace seamcarve {

   QImage do_seam_carving(QImage image, QSize old_size, QSize size);
   QImage calculate_image_energy(QImage image);
}

#endif
