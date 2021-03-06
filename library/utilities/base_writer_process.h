/*ckwg +5
 * Copyright 2011-2015 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef vidtk_base_writer_process_h_
#define vidtk_base_writer_process_h_

#include <utilities/base_io_process.h>


namespace vidtk
{


// ----------------------------------------------------------------
/** Writer process abstract base class.
 *
 * This class represents the base class for processes that write
 * data to a file.
 *
 */
class base_writer_process
  : public base_io_process
{
public:
  // typedef base_writer_process self_type;

  base_writer_process( std::string const& name, std::string const& type );
  virtual ~base_writer_process();


protected:
  virtual bool initialize_internal();

private:
  void open_file();
  int do_file_step();

};

} // end namespace vidtk

#endif // vidtk_base_writer_process_h_
