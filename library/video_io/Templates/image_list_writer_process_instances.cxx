/*ckwg +5
 * Copyright 2010-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include <video_io/image_list_writer_process.txx>

template class vidtk::image_list_writer_process<float>;
template class vidtk::image_list_writer_process<double>;
template class vidtk::image_list_writer_process<bool>;
template class vidtk::image_list_writer_process<unsigned>;
template class vidtk::image_list_writer_process<vxl_byte>;
template class vidtk::image_list_writer_process<vxl_uint_16>;
