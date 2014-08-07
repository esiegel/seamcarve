#ifndef SEAMCARVE_HPP
#define SEAMCARVE_HPP

#include <QtCore/QSize>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

namespace seamcarve {

   QImage resize(QImage image, QSize size);
   QImage calculate_energy_image(QImage image);
}

#endif
