#ifndef SEAMCARVE_HPP
#define SEAMCARVE_HPP

#include <QtCore/QSize>
#include <QtGui/QImage>

namespace seamcarve {

   QImage do_seam_carving(QImage image, QSize old_size, QSize size);

}

#endif
