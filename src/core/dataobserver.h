/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef DATAOBSERVER_H
#define DATAOBSERVER_H
#include "compilersettings.h"
#include <boost/shared_ptr.hpp>
#include <string>
//#include "module.h"
#include <rasterdata.h>

namespace vibens {
class Module;

//using namespace boost;
using namespace std;

/**
  * after the return of each observe callback the data may be invalid!!
  */

class VIBE_HELPER_DLL_EXPORT DataObserver
{
public:
    DataObserver();
    virtual void observeNewRasterData(RasterData) = 0;
   // virtual void observeNewVectorData(Module * module, string name) = 0;

    //virtual void observerDeletedRasterData(Module *  module) = 0;
    //virtual void observerDeletedVectorData(Module *   module) = 0;
};
}
#endif // DATAOBSERVER_H